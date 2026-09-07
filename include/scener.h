#ifndef SCENER_H
#define SCENER_H

#ifdef __cplusplus
extern "C"
{
#endif
#define SCENE_MAX_FILE_SIZE 1024 * 1024 // 1MB
#define SCENE_MAX_LINE_LENGHT 256
#define SCENE_MAX_FILE_NAME 256

    int scener_run_file(const char *filename);

#ifdef __cplusplus
}
#endif

#endif /* SCENER_H */