#pragma once

#ifdef EULER_EXPORTS
	#define EULER_API __declspec(dllexport)
#elif
	#define EULER_API __declspec(dllimport)
#endif // EULER_EXPORTS


#define EULER_ENGINE_NAME "Euler Engine"
#define EULER_MAJOR 0
#define EULER_MINOR 1
#define EULER_PATCH 0