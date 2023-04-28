#ifndef GLSTUFF_SCENE_DEMO_HPP
#define GLSTUFF_SCENE_DEMO_HPP

namespace scene_demos {
	
	class scene_demo {
	public:
		scene_demo() {}
		virtual ~scene_demo() {}
		virtual void render(float delta) {}
	};
};

#endif //GLSTUFF_SCENE_DEMO_HPP
