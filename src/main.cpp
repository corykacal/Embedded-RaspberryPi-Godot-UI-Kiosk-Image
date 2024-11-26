#include <GLES2/gl2.h>
#include <EGL/egl.h>
#include <EGL/eglext.h>
#include <gbm.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <linux/vt.h>
#include <linux/kd.h>
#include <sys/ioctl.h>
#include <xf86drm.h>
#include <xf86drmMode.h>

const char* vertex_shader_source =
    "attribute vec4 position;\n"
    "void main() {\n"
    "    gl_Position = position;\n"
    "}\n";

const char* fragment_shader_source =
    "precision mediump float;\n"
    "void main() {\n"
    "    gl_FragColor = vec4(1.0, 1.0, 0.0, 1.0);\n"
    "}\n";

void cleanup_console() {
    // Open current TTY
    int tty_fd = open("/dev/tty1", O_RDWR);
    if (tty_fd >= 0) {
        // Restore text mode
        ioctl(tty_fd, KDSETMODE, KD_TEXT);
        close(tty_fd);
    }
}

int main() {
    // Disable terminal
    int tty_fd = open("/dev/tty1", O_RDWR);
    if (tty_fd >= 0) {
        // Set graphics mode
        ioctl(tty_fd, KDSETMODE, KD_GRAPHICS);

        // Disable cursor
        printf("\033[?25l");
        fflush(stdout);
    }

    // Register cleanup handler
    atexit(cleanup_console);

    // Open DRM device
    int fd = open("/dev/dri/card0", O_RDWR);
    if (fd < 0) {
        printf("Failed to open DRM device\n");
        return 1;
    }

    // Create GBM device
    struct gbm_device* gbm = gbm_create_device(fd);
    if (!gbm) {
        printf("Failed to create GBM device\n");
        close(fd);
        return 1;
    }

    // Create surface
    struct gbm_surface* gbm_surface = gbm_surface_create(
        gbm,
        1920, 1080,
        GBM_FORMAT_XRGB8888,
        GBM_BO_USE_SCANOUT | GBM_BO_USE_RENDERING
    );

    // Get EGL display
    PFNEGLGETPLATFORMDISPLAYEXTPROC get_platform_display =
        (PFNEGLGETPLATFORMDISPLAYEXTPROC)eglGetProcAddress("eglGetPlatformDisplayEXT");

    EGLDisplay display = get_platform_display(EGL_PLATFORM_GBM_KHR, gbm, NULL);
    if (display == EGL_NO_DISPLAY) {
        printf("Failed to get EGL display\n");
        return 1;
    }

    // Initialize EGL
    EGLint major, minor;
    if (!eglInitialize(display, &major, &minor)) {
        printf("Failed to initialize EGL\n");
        return 1;
    }

    // Configure EGL
    const EGLint config_attribs[] = {
        EGL_SURFACE_TYPE, EGL_WINDOW_BIT,
        EGL_RED_SIZE, 8,
        EGL_GREEN_SIZE, 8,
        EGL_BLUE_SIZE, 8,
        EGL_ALPHA_SIZE, 0,
        EGL_RENDERABLE_TYPE, EGL_OPENGL_ES2_BIT,
        EGL_NONE
    };

    EGLConfig config;
    EGLint num_config;
    eglChooseConfig(display, config_attribs, &config, 1, &num_config);

    // Create EGL context
    const EGLint context_attribs[] = {
        EGL_CONTEXT_CLIENT_VERSION, 2,
        EGL_NONE
    };

    EGLContext context = eglCreateContext(display, config, EGL_NO_CONTEXT, context_attribs);
    EGLSurface surface = eglCreateWindowSurface(display, config, (EGLNativeWindowType)gbm_surface, NULL);

    eglMakeCurrent(display, surface, surface, context);

    // Set up GL resources
    GLuint vertex_shader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertex_shader, 1, &vertex_shader_source, NULL);
    glCompileShader(vertex_shader);

    GLuint fragment_shader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragment_shader, 1, &fragment_shader_source, NULL);
    glCompileShader(fragment_shader);

    GLuint program = glCreateProgram();
    glAttachShader(program, vertex_shader);
    glAttachShader(program, fragment_shader);
    glLinkProgram(program);
    glUseProgram(program);

    // Create vertex buffer
    float vertices[] = {
        -1.0f, -1.0f,
         1.0f, -1.0f,
         1.0f,  1.0f,
        -1.0f,  1.0f
    };

    GLuint vbo;
    glGenBuffers(1, &vbo);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    GLint pos_attrib = glGetAttribLocation(program, "position");
    glEnableVertexAttribArray(pos_attrib);
    glVertexAttribPointer(pos_attrib, 2, GL_FLOAT, GL_FALSE, 0, 0);

    // Main render loop
    while (1) {
        glClear(GL_COLOR_BUFFER_BIT);
        glDrawArrays(GL_TRIANGLE_FAN, 0, 4);
        eglSwapBuffers(display, surface);

        struct gbm_bo* bo = gbm_surface_lock_front_buffer(gbm_surface);
        uint32_t handle = gbm_bo_get_handle(bo).u32;
        uint32_t pitch = gbm_bo_get_stride(bo);
        uint32_t fb_id;

        drmModeAddFB(fd, 1920, 1080, 24, 32, pitch, handle, &fb_id);
        drmModeSetCrtc(fd, 0, fb_id, 0, 0, NULL, 0, NULL);

        drmModeRmFB(fd, fb_id);
        gbm_surface_release_buffer(gbm_surface, bo);
    }

    return 0;
}