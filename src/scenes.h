#ifndef __SCENES_H__
#define __SCENES_H__

// Define the scene names and functions here
// Let the macro tricks handle the rest

// parameters: function name, enum name, string name
#define SCENES\
    S(title, TITLE, "Title")\
    S(bouncing_ball, BOUNCING_BALL, "Bouncing Ball")\
    S(simple_pendulum, SIMPLE_PENDULUM, "Simple Pendulum")\
    S(double_pendulum, DOUBLE_PENDULUM, "Double Pendulum")\

// Functions defined for every scene
// parameters: name of the scene (sometimes not used)
#define SCENE_FUNCS(NAME)\
    F(void, NAME, init)\
    F(void, NAME, update, float)\
    F(void, NAME, render)\
    F(void, NAME, unload)\
    F(SceneNum, NAME, transition)

///////////////////////////////////////////////////////////////
/// Macro black magic
///////////////////////////////////////////////////////////////

// Array of scene names
#define S(FUNC, ENUM, STR) STR,
static const char* SCENE_NAMES[] = {
    SCENES
};
#undef S

// Assign each scene an enum
#define S(FUNC, ENUM, STR) ENUM,

typedef enum {
    SCENES
    SCENE_CNT,
} SceneNum;

#undef S


// Declare all the scene functions
#define F(RETURN, NAME, SUFFIX, ...) RETURN NAME##_##SUFFIX(__VA_ARGS__);
#define S(FUNC, ENUM, STR) SCENE_FUNCS(FUNC)

SCENES

#undef S
#undef F


// Define the scene structure for the scene array
#define F(RETURN, NAME, SUFFIX, ...) RETURN (*SUFFIX)(__VA_ARGS__);

typedef struct {
    const char* name;
    SCENE_FUNCS()
} Scene;

#undef F


// Define the macro to instantiate the scenes[] array used in the application
// parameters: the name of the scene array
// Note that this should be last in the file
// because the S() and F() macro cannot be redefined
#define F(RETURN, NAME, SUFFIX, ...) .SUFFIX = NAME##_##SUFFIX,
#define S(FUNC, ENUM, STR) (Scene){ .name = STR, SCENE_FUNCS(FUNC) },

#define INIT_SCENE_ARR(NAME)\
    Scene NAME[] = {\
        SCENES\
    };\

#endif // __SCENES_H__
