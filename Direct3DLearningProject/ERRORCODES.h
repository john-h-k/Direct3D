#pragma once

// TODO docs

/**
 * \brief Return code for a successful execution
 */
#define SUCCEED 0x00
/**
 * \brief Return code for failure during initialization
 * of window
 */
#define FAIL_WINDOW_INIT 0x01
/**
 * \brief Return code for failure during initialization
 * of any part of the render
 */
#define FAIL_DIRECT3D_INIT 0x02
/**
 * \brief Return code for failure during initialization
 * of the renderer scene
 */
#define FAIL_DIRECT3D_SCENE_INIT 0x03
/**
 * \brief Return code for failure during window destruction
 */
#define FAIL_WINDOW_DESTROY 0x04