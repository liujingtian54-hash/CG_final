#include"core.h"
#include"scene.h"

//TODO:core全部的实现
Core::Core(const Options& options) : Application(options), _camera(1.2,1.0,-1.0,1.0){
	Scene* scene = new Scene;
}