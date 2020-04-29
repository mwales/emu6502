#ifndef FORCE_EXECUTE_H
#define FORCE_EXECUTE_H

/** 

 * This macro will abuse support for static C++ object constructors to give
 * the caller a function that will be executed if the file is compiled and
 * linked into the application
 */
#define FORCE_EXECUTE(className) \
	class className { \
		public: \
			className(); \
	}; \
	static className className ## _instance; \
	className::className() \

#endif
