#include <GAL/graphics/gltf_loader.hpp>
#include "glm/gtc/quaternion.hpp"
#include "glm/gtc/type_ptr.hpp"

#include <unordered_map>
#include <unordered_set>
#include <list>
#include <scluk/log.hpp>
#include <scluk/aliases.hpp>
#include <scluk/read_file.hpp>
#include <tinygltf/tiny_gltf.h>

using namespace std;

static tinygltf::Model load_gltf_from_file(const char *path, bool binary) {
    tinygltf::TinyGLTF loader;
    tinygltf::Model model;
    string err, warn;

    bool success =
        binary ? loader.LoadBinaryFromFile(&model, &err, &warn, path) // for .glb
               : loader.LoadASCIIFromFile(&model, &err, &warn, path); // for .gltf

    if(!warn.empty())
        scluk::stdout_log.warn("{}", warn);

    if(!err.empty())
        scluk::stdout_log.error("{}", err);

    if(!success)
        throw runtime_error("Fatal error: Failed to parse glTF");

    return model;
}

static void deduce_vbo_strides(vector<gal::graphics::vertex_buffer>& vbos, const gal::graphics::vertex_layout& vertex_layout) {
    unordered_set<size_t> vbos_to_deduce_stride_of;
    for(size_t i = 0; i < vbos.size(); i++) {
        if(vbos[i].get_stride() == 0)
            vbos_to_deduce_stride_of.insert(i);
    }

    for(const auto& attrib : vertex_layout.attribs) {
        size_t vbo_index = attrib.vao_vbo_bind_index;
        if(vbos_to_deduce_stride_of.contains(vbo_index)) {
            size_t attrib_size = attrib.size * gal::graphics::typeid_to_size(attrib.type_id);
            vbos[vbo_index].set_stride(vbos[vbo_index].get_stride() + attrib_size);
        }
    }
}
static gal::graphics::vertex_buffer make_vbo_from_bufview(const tinygltf::Model& model, int bufview_idx) {
    auto& bufview = model.bufferViews[bufview_idx];
    auto& buf = model.buffers[bufview.buffer];
    assert(bufview.target == TINYGLTF_TARGET_ARRAY_BUFFER);

    return gal::graphics::vertex_buffer(&buf.data[bufview.byteOffset], bufview.byteLength, bufview.byteStride);
}

static gal::graphics::index_buffer make_ibo_from_bufview(const tinygltf::Model& model, int bufview_idx, int element_typeid) {
    auto& bufview = model.bufferViews[bufview_idx];
    auto& buf = model.buffers[bufview.buffer];
    assert(bufview.target == TINYGLTF_TARGET_ELEMENT_ARRAY_BUFFER);

    gal::graphics::buffer raw_data(&buf.data[bufview.byteOffset], bufview.byteLength);
    int triangle_size = gal::graphics::typeid_to_size(element_typeid) * 3;
    int triangle_count = bufview.byteLength / triangle_size;

    return gal::graphics::index_buffer(std::move(raw_data), triangle_count, element_typeid);
}

static gal::graphics::vertex_array get_vao(const tinygltf::Model& model, int mesh_idx) {
    const tinygltf::Mesh& mesh = model.meshes[mesh_idx];

    vector<gal::graphics::vertex_buffer> vbos;
    unordered_map<int, int> bufview_to_vbo_map;

    //TODO: currently only 1 primitive per mesh is supported, for more we would need multiple renderables (and draw calls) for a single node which we do not currently support. maybe for the better
    assert(mesh.primitives.size() == 1);
    const tinygltf::Primitive& primitive = mesh.primitives[0];
    //TODO: support other modes other than TRIANGLES
    assert(primitive.mode == TINYGLTF_MODE_TRIANGLES);
    using vertex_array_attrib = gal::graphics::vertex_layout::vertex_array_attrib;
    gal::graphics::vertex_layout layout;

    //populate vbos and layout.attribs
    for (auto& [attrib_name, accessor_idx] : primitive.attributes) {
        //vertex array attribute
        int vaa =
            attrib_name == "POSITION" ? 0 :
            attrib_name == "TEXCOORD_0" ? 1 : -1;

        if(vaa != -1) {
            const tinygltf::Accessor& accessor = model.accessors[accessor_idx];
            int bufview_idx = accessor.bufferView;

            int vbo_bind = -1;

            if (!bufview_to_vbo_map.contains(bufview_idx)) {
                bufview_to_vbo_map[bufview_idx] = vbos.size();
                vbo_bind = vbos.size();
                vbos.push_back(make_vbo_from_bufview(model, bufview_idx));
            }

            //TODO: support matrices: this currently only works for scalars and vec2/3/4
            int size = accessor.type == TINYGLTF_TYPE_SCALAR ?  1 : accessor.type;

            if (vbo_bind == -1) // only lookup in the table if we didn't just add the element into it
                vbo_bind = bufview_to_vbo_map[bufview_idx];

            vertex_array_attrib attrib(vaa, accessor.byteOffset, accessor.componentType, size, vbo_bind, accessor.normalized);
            layout.attribs.push_back(attrib);
        }
    }

    deduce_vbo_strides(vbos, layout);

    // TODO: currently only supporting indexed data; if primitive.indices is undefined(-1) then the primitive is non-indexed
    assert(primitive.indices != -1);
    const tinygltf::Accessor& indices_accessor = model.accessors[primitive.indices];
    // currently the only types supported by gal::graphics::index_buffer are unsigned int and unsigned short
    assert(indices_accessor.componentType == TINYGLTF_COMPONENT_TYPE_UNSIGNED_INT
        || indices_accessor.componentType == TINYGLTF_COMPONENT_TYPE_UNSIGNED_SHORT);
    gal::graphics::index_buffer ibo = make_ibo_from_bufview(model, indices_accessor.bufferView, indices_accessor.componentType);
    vector<gal::graphics::index_buffer> ibos;
    ibos.push_back(std::move(ibo));

    return gal::graphics::vertex_array(std::move(vbos), std::move(ibos), std::move(layout));
}

static gal::graphics::texture get_mesh_texture(const tinygltf::Model& model, int mesh_idx) {
    //TODO: currently only 1 primitive per mesh is supported, for more we would need multiple renderables (and draw calls) for a single node which we do not currently support. maybe for the better
    assert(model.meshes[mesh_idx].primitives.size() == 1);
    int material_idx = model.meshes[mesh_idx].primitives[0].material;
    assert(material_idx != -1);
    int texture_idx = model.materials[material_idx].pbrMetallicRoughness.baseColorTexture.index;
    assert(texture_idx != -1);
    int image_idx = model.textures[texture_idx].source;
    assert(image_idx != -1);
    const tinygltf::Image& image = model.images[image_idx];

    assert(image.bits == 8);
    assert(image.component == 4); // TODO: remove this assert

    gal::graphics::texture::specification spec {
        .w = image.width, .h = image.height,
        .components = image.component,
        .data = image.image.data(),
        .alignment = 1,
        .repeat_wrap = false,
    };
    return gal::graphics::texture(spec);

}

static gal::graphics::retro::retro_renderable load_renderable(const tinygltf::Model& model, int mesh_idx) {
    //TODO: does the renderable also need the index of any textures or does it already have them

    gal::graphics::vertex_array vao = get_vao(model, mesh_idx);

    gal::graphics::texture texture = get_mesh_texture(model, mesh_idx);

    return gal::graphics::retro::retro_renderable(std::move(vao), std::move(texture));
}

static glm::mat4 get_node_transform(const tinygltf::Node& n) {
    using namespace glm;

    mat4 rotation_mat = n.rotation.empty() ? mat4(1) : mat4_cast((quat)make_quat(n.rotation.data()));
    mat4 scale_mat = n.scale.empty() ? mat4(1) : scale(mat4(1), (vec3)make_vec3(n.scale.data()));
    mat4 translation_mat = n.translation.empty() ? mat4(1) : translate(mat4(1), vec3(n.translation[0], n.translation[1], n.translation[2]));
    mat4 raw_mat = n.matrix.empty() ? mat4(1) : (mat4)make_mat4(n.matrix.data());

    return translation_mat * scale_mat * rotation_mat * raw_mat;
}

static gal::application::retro::node load_node_subtree(const tinygltf::Model& model, int idx) {
    const tinygltf::Node& node = model.nodes[idx];

    shared_ptr<gal::graphics::retro::retro_renderable> mesh;
    if (node.mesh != -1) {
        mesh = make_shared<gal::graphics::retro::retro_renderable>(load_renderable(model, node.mesh));
    }

    glm::mat4 transform = get_node_transform(node);
    gal::application::retro::node root(node.name, std::move(mesh), transform);

    for(int child_idx : node.children) {
        root.add_child(load_node_subtree(model, child_idx));
    }

    return root;
}

gal::application::retro::node load_node_tree_from_gltf(const char* filepath, const char* node_name) {
    bool binary = string_view(filepath).ends_with(".glb");
    const tinygltf::Model model = load_gltf_from_file(filepath, binary);

    gal::application::retro::node root(node_name ? node_name : filepath);

    const tinygltf::Scene& scene = model.scenes.at(0);
    list<int> node_idx_queue;
    for (int node_idx : scene.nodes)
        root.add_child(load_node_subtree(model, node_idx));

    return root;
}
