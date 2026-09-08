#define _DEFAULT_SOURCE

#include <stdio.h>
#include <string.h>
#include <vdp_api.h>
#include <scener.h>
#include <unistd.h>
#define SCENE_NUMBER_OF_COMMANDS 6
#define SCENE_CMD_ENABLE 0
#define SCENE_CMD_DISABLE 1
#define SCENE_CMD_LOAD 2
#define SCENE_CMD_LOAD_SPRITE 3
#define SCENE_CMD_ACTION 4
#define SCENE_CMD_WAIT 5

int cmd_get_int_arg(
    char *scene_line,
    int *arg,
    const char *filename,
    int line_num);

char *commands[SCENE_NUMBER_OF_COMMANDS] = {
    "enable",
    "disable",
    "load",
    "load_sprite",
    "action",
    "wait"};

void cmd_enable(char *scene_line, const char *filename, int line_num)
{
    scene_line = strtok(NULL, ":"); // Get argument

    if (scene_line == NULL)
    {
        printf("%s:%d: Invalid argument to command\n", filename, line_num);
        return;
    }

    if (strcmp(scene_line, "B0") == 0)
        vdp_b0_enable();
    else if (strcmp(scene_line, "T0") == 0)
        vdp_t0_enable();
    else if (strcmp(scene_line, "T1") == 0)
        vdp_t1_enable();
    else if (strcmp(scene_line, "S0") == 0)
        vdp_s0_enable();
    else
        printf("%s:%d: Invalid layer: %s\n", filename, line_num, scene_line);
}

void cmd_disable(char *scene_line, const char *filename, int line_num)
{
    scene_line = strtok(NULL, ":"); // Get argument

    if (scene_line == NULL)
    {
        printf("%s:%d: Invalid argument to command\n", filename, line_num);
        return;
    }

    if (strcmp(scene_line, "B0") == 0)
        vdp_b0_disable();
    else if (strcmp(scene_line, "T0") == 0)
        vdp_t0_disable();
    else if (strcmp(scene_line, "T1") == 0)
        vdp_t1_disable();
    else if (strcmp(scene_line, "S0") == 0)
        vdp_s0_disable();
    else
        printf("%s:%d: Invalid layer: %s\n", filename, line_num, scene_line);
}

void cmd_wait(char *scene_line, const char *filename, int line_num)
{
    scene_line = strtok(NULL, ":"); // Get argument

    if (scene_line == NULL)
    {
        printf("%s:%d: Invalid argument to command\n", filename, line_num);
        return;
    }

    int delay = 0;
    int ret = sscanf(scene_line, "%d", &delay);
    if (ret != 1 || delay < 0)
    {
        printf("%s:%d: Invalid argument to command\n", filename, line_num);
        return;
    }

    usleep(delay * 1000);
}

void cmd_load(char *scene_line, const char *filename, int line_num)
{
    scene_line = strtok(NULL, ":"); // Get argument

    if (scene_line == NULL)
    {
        printf("%s:%d: Invalid argument to command\n", filename, line_num);
        return;
    }

    int layer = -1;
    if (strcmp(scene_line, "B0") == 0)
        layer = 0;
    else if (strcmp(scene_line, "T0") == 0)
        layer = 1;
    else if (strcmp(scene_line, "T1") == 0)
        layer = 2;
    else
        printf("%s:%d: Invalid layer: %s\n", filename, line_num, scene_line);

    char *fn = malloc(SCENE_MAX_FILE_NAME * sizeof(char));
    if (fn == NULL)
    {
        perror("SCENER: Failed to do malloc");
        return;
    }

    scene_line = strtok(NULL, ":"); // Get argument

    if (scene_line == NULL)
    {
        printf("%s:%d: Invalid argument to command\n", filename, line_num);
        return;
    }

    int ret = sscanf(scene_line, "%s", fn);
    if (ret != 1 || fn == NULL)
    {
        printf("%s:%d: Invalid argument to command\n", filename, line_num);
        free(fn);
        return;
    }

    int x_off, y_off;
    int load_pal, off, cnt;
    switch (layer)
    {
    case 0:
        if (cmd_get_int_arg(scene_line, &x_off, filename, line_num) != 0)
            return;
        if (cmd_get_int_arg(scene_line, &y_off, filename, line_num) != 0)
            return;
        if (vdp_b0_load_b0_file(fn, x_off, y_off))
        {
            printf("%s:%d: Error loading file\n", filename, line_num);
        }
        break;
    case 1:
        // T0
        if (cmd_get_int_arg(scene_line, &off, filename, line_num) != 0)
            return;
        if (cmd_get_int_arg(scene_line, &cnt, filename, line_num) != 0)
            return;
        if (cmd_get_int_arg(scene_line, &load_pal, filename, line_num) != 0)
            return;
        vdp_t0_load_til_file(fn, off, cnt, load_pal);
        break;

    case 2:
        // T1
        if (cmd_get_int_arg(scene_line, &off, filename, line_num) != 0)
            return;
        if (cmd_get_int_arg(scene_line, &cnt, filename, line_num) != 0)
            return;
        if (cmd_get_int_arg(scene_line, &load_pal, filename, line_num) != 0)
            return;
        vdp_t1_load_til_file(fn, off, cnt, load_pal);
        break;

    default:
        break;
    }
    free(fn);
}

// Return 0 on sucsess and prints error
int cmd_get_int_arg(char *scene_line, int *arg, const char *filename, int line_num)
{
    scene_line = strtok(NULL, ":"); // Get argument

    if (scene_line == NULL)
    {
        printf("%s:%d: Invalid argument to command\n", filename, line_num);
        return -1;
    }

    int ret = sscanf(scene_line, "%d", arg);
    if (ret != 1)
    {
        printf("%s:%d: Invalid argument to command\n", filename, line_num);
        return -1;
    }
    return 0;
}
void cmd_sprite_load(char *scene_line, const char *filename, int line_num)
{
    scene_line = strtok(NULL, ":"); // Get argument

    if (scene_line == NULL)
    {
        printf("%s:%d: Invalid argument to command\n", filename, line_num);
        return;
    }
    if (strcmp(scene_line, "S0") != 0)
        printf("%s:%d: Invalid layer: %s\n", filename, line_num, scene_line);

    char *fn = malloc(SCENE_MAX_FILE_NAME * sizeof(char));
    if (fn == NULL)
    {
        perror("SCENER: Failed to do malloc");
        return;
    }

    scene_line = strtok(NULL, ":"); // Get argument

    if (scene_line == NULL)
    {
        printf("%s:%d: Invalid argument to command\n", filename, line_num);
        free(fn);
        return;
    }

    int ret = sscanf(scene_line, "%s", fn);
    if (ret != 1 || fn == NULL)
    {
        printf("%s:%d: Invalid argument to command\n", filename, line_num);
        free(fn);
        return;
    }

    int offset, cnt, size, pal_num;
    if (cmd_get_int_arg(scene_line, &offset, filename, line_num) != 0)
    {
        free(fn);
        return;
    }

    if (cmd_get_int_arg(scene_line, &cnt, filename, line_num) != 0)
    {
        free(fn);
        return;
    }

    if (cmd_get_int_arg(scene_line, &size, filename, line_num) != 0)
    {
        free(fn);
        return;
    }

    if (cmd_get_int_arg(scene_line, &pal_num, filename, line_num) != 0)
    {
        free(fn);
        return;
    }

    printf("off: %d, cnt: %d, size: %d, pal_num: %d\n", offset, cnt, size, pal_num);
    if (size != 16 && size != 32)
    {
        printf("%s:%d: Invalid argument size to command\n", filename, line_num);
        free(fn);
        return;
    }

    if (offset < 0 || offset > 32768 - (size * size))
    {
        printf("%s:%d: Invalid argument offset to command\n", filename, line_num);
        free(fn);
        return;
    }

    if ((size * size * cnt) + offset > 32768)
    {
        printf("%s:%d: Out of VDP memory to command\n", filename, line_num);
        free(fn);
        return;
    }

    if (pal_num > 3 || pal_num < 0)
    {
        printf("%s:%d: Invalid argument pal_num to command\n", filename, line_num);
        free(fn);
        return;
    }

    ret = vdp_s0_load_spr_file(fn, offset, cnt, pal_num, size == 16 ? 0 : 1);
    if (ret != 0)
    {
        printf("%s:%d: Error loading sprite data\n", filename, line_num);
        free(fn);
        return;
    }
    free(fn);
}

void do_sprite_action(char *scene_line, const char *filename, int line_num)
{
    scene_line = strtok(NULL, ":"); // Get argument

    if (scene_line == NULL)
    {
        printf("%s:%d: Invalid argument to command\n", filename, line_num);
        return;
    }

    int action = -1;

    if (strcmp(scene_line, "sprite_activate") == 0)
        action = 1;
    else if (strcmp(scene_line, "sprite_deactivate") == 0)
        action = 2;
    else if (strcmp(scene_line, "sprite_set") == 0)
        action = 3;
    else if (strcmp(scene_line, "sprite_move") == 0)
        action = 4;
    else if (strcmp(scene_line, "sprite_h_flip") == 0)
        action = 5;
    else if (strcmp(scene_line, "sprite_v_flip") == 0)
        action = 6;
    else if (strcmp(scene_line, "sprite_scale") == 0)
        action = 7;
    else if (strcmp(scene_line, "sprite_frame") == 0)
        action = 8;
    else
    {
        printf("%s:%d: Invalid argument to command\n", filename, line_num);
        return;
    }

    int spr_num;
    if (cmd_get_int_arg(scene_line, &spr_num, filename, line_num))
    {
        return;
    }

    if (spr_num < 0 || spr_num > 63)
    {
        printf("%s:%d: Invalid argument spr_num to command\n", filename, line_num);
        return;
    }
    sprite_attribute_t spr_att;
    int x_pos, y_pos, h_flip, v_flip, size, scale;
    int x_delta, y_delta, frame, offset;
    switch (action)
    {
    case 1:

        spr_att = vdp_s0_read_sprite_attribute(spr_num);
        spr_att.active = 1;
        vdp_s0_write_sprite_attribute(spr_att, spr_num);
        break;

    case 2:
        spr_att = vdp_s0_read_sprite_attribute(spr_num);
        spr_att.active = 0;
        vdp_s0_write_sprite_attribute(spr_att, spr_num);
        break;
    case 3:
        //`sprite_set:<sprite number>:<x_pos>:<y_pos>:<h_flip 1 or 0>:<v_flip 1 or 0>:<size(16 or 32)>:<scale(0 - 3)>

        if (cmd_get_int_arg(scene_line, &x_pos, filename, line_num))
        {
            return;
        }
        if (cmd_get_int_arg(scene_line, &y_pos, filename, line_num))
        {
            return;
        }
        if (cmd_get_int_arg(scene_line, &h_flip, filename, line_num))
        {
            return;
        }
        if (cmd_get_int_arg(scene_line, &v_flip, filename, line_num))
        {
            return;
        }
        if (cmd_get_int_arg(scene_line, &size, filename, line_num))
        {
            return;
        }
        if (cmd_get_int_arg(scene_line, &scale, filename, line_num))
        {
            return;
        }

        if ((size != 16 && size != 32) || scale < 0 || scale > 3)
        {
            printf("%s:%d: Invalid argument spr_num to command\n", filename, line_num);
            return;
        }

        spr_att = vdp_s0_read_sprite_attribute(spr_num);
        spr_att.x_pos = x_pos;
        spr_att.y_pos = y_pos;
        spr_att.scale = scale;
        spr_att.size = size == 32 ? 1 : 0;
        spr_att.h_flip = h_flip;
        spr_att.v_flip = v_flip;
        vdp_s0_write_sprite_attribute(spr_att, spr_num);
        break;

    case 4:
        // sprite_move

        if (cmd_get_int_arg(scene_line, &x_delta, filename, line_num))
        {
            return;
        }
        if (cmd_get_int_arg(scene_line, &y_delta, filename, line_num))
        {
            return;
        }
        spr_att = vdp_s0_read_sprite_attribute(spr_num);
        spr_att.x_pos += x_delta;
        spr_att.y_pos += y_delta;
        vdp_s0_write_sprite_attribute(spr_att, spr_num);
        break;

    case 5:
        // sprite_h_flip
        if (cmd_get_int_arg(scene_line, &h_flip, filename, line_num))
        {
            return;
        }
        if (h_flip != 0 && h_flip != 1)
        {
            printf("%s:%d: Invalid argument h_flip to command\n", filename, line_num);
            return;
        }
        spr_att = vdp_s0_read_sprite_attribute(spr_num);
        spr_att.h_flip = h_flip;
        vdp_s0_write_sprite_attribute(spr_att, spr_num);
        break;

    case 6:
        // sprite_v_flip
        if (cmd_get_int_arg(scene_line, &v_flip, filename, line_num))
        {
            return;
        }
        if (v_flip != 0 && v_flip != 1)
        {
            printf("%s:%d: Invalid argument v_flip to command\n", filename, line_num);
            return;
        }
        spr_att = vdp_s0_read_sprite_attribute(spr_num);
        spr_att.v_flip = v_flip;
        vdp_s0_write_sprite_attribute(spr_att, spr_num);
        break;

    case 7:
        // sprite_scale
        if (cmd_get_int_arg(scene_line, &scale, filename, line_num))
        {
            return;
        }
        if (scale < 0 || scale > 3)
        {
            printf("%s:%d: Invalid argument scale to command\n", filename, line_num);
            return;
        }
        spr_att = vdp_s0_read_sprite_attribute(spr_num);
        spr_att.scale = scale;
        vdp_s0_write_sprite_attribute(spr_att, spr_num);
        break;

    case 8:
        // sprite_frame
        if (cmd_get_int_arg(scene_line, &offset, filename, line_num))
        {
            return;
        }

        if (cmd_get_int_arg(scene_line, &frame, filename, line_num))
        {
            return;
        }
        spr_att = vdp_s0_read_sprite_attribute(spr_num);
        offset += frame * (spr_att.size * spr_att.size);
        if (offset > 32768 - (spr_att.size * spr_att.size))
        {
            printf("%s:%d: Invalid argument offset or frame to command\n", filename, line_num);
            return;
        }

        spr_att.offset = offset;
        vdp_s0_write_sprite_attribute(spr_att, spr_num);
        break;
    default:
        break;
    }
}
void do_normal_action(char *scene_line, const char *filename, int line_num, int layer)
{
    scene_line = strtok(NULL, ":"); // Get argument

    if (scene_line == NULL)
    {
        printf("%s:%d: Invalid argument to command\n", filename, line_num);
        return;
    }

    int do_x_delta = 0;
    int do_x_offset = 0;
    int do_y_delta = 0;
    int do_y_offset = 0;

    if (strcmp(scene_line, "scroll_x_set") == 0)
        do_x_offset = 1;
    else if (strcmp(scene_line, "scroll_y_set") == 0)
        do_y_offset = 1;
    else if (strcmp(scene_line, "scroll_x_delta") == 0)
        do_x_delta = 1;
    else if (strcmp(scene_line, "scroll_y_delta") == 0)
        do_y_delta = 1;
    else
    {
        printf("%s:%d: Invalid argument to command\n", filename, line_num);
        return;
    }
    int amaunt = 0;

    if (cmd_get_int_arg(scene_line, &amaunt, filename, line_num) != 0)
    {
        printf("%s:%d: Invalid argument to command\n", filename, line_num);
        return;
    }

    if (do_x_delta || do_y_delta)
    {
        if (amaunt > 1024 || amaunt < -1024)
        {
            printf("%s:%d: Integer out of range\n", filename, line_num);
            return;
        }
    }

    if (do_x_offset || do_y_offset)
    {
        if (amaunt > 1024 || amaunt < 0)
        {
            printf("%s:%d: Integer out of range\n", filename, line_num);
            return;
        }
    }

    if (do_x_offset)
    {
        switch (layer)
        {
        case 0:
            vdp_b0_set_x_offset(amaunt);
            break;

        case 1:
            vdp_t0_set_x_offset(amaunt);
            break;

        case 2:
            vdp_t1_set_x_offset(amaunt);
            break;

        default:
            break;
        }
    }
    else if (do_y_offset)
    {
        switch (layer)
        {
        case 0:
            vdp_b0_set_y_offset(amaunt);
            break;

        case 1:
            vdp_t0_set_y_offset(amaunt);
            break;

        case 2:
            vdp_t1_set_y_offset(amaunt);
            break;

        default:
            break;
        }
    }
    else if (do_x_delta)
    {
        switch (layer)
        {
        case 0:
            vdp_b0_set_x_offset(vdp_b0_get_x_offset() - amaunt);
            break;

        case 1:
            vdp_t0_set_x_offset(vdp_t0_get_x_offset() - amaunt);
            break;

        case 2:
            vdp_t1_set_x_offset(vdp_t1_get_x_offset() - amaunt);
            break;

        default:
            break;
        }
    }
    else if (do_y_delta)
    {
        switch (layer)
        {
        case 0:
            vdp_b0_set_y_offset(vdp_b0_get_y_offset() - amaunt);
            break;

        case 1:
            vdp_t0_set_y_offset(vdp_t0_get_y_offset() - amaunt);
            break;

        case 2:
            vdp_t1_set_y_offset(vdp_t1_get_y_offset() - amaunt);
            break;

        default:
            break;
        }
    }
}

void cmd_action(char *scene_line, const char *filename, int line_num)
{
    scene_line = strtok(NULL, ":"); // Get argument

    if (scene_line == NULL)
    {
        printf("%s:%d: Invalid argument to command\n", filename, line_num);
        return;
    }
    if (strcmp(scene_line, "S0") == 0)
        do_sprite_action(scene_line, filename, line_num);
    else if (strcmp(scene_line, "B0") == 0)
        do_normal_action(scene_line, filename, line_num, 0);
    else if (strcmp(scene_line, "T0") == 0)
        do_normal_action(scene_line, filename, line_num, 1);
    else if (strcmp(scene_line, "T1") == 0)
        do_normal_action(scene_line, filename, line_num, 2);
    else
        printf("%s:%d: Invalid layer: %s\n", filename, line_num, scene_line);
}

int scener_run_file(const char *filename)
{
    FILE *fd;
    fd = fopen(filename, "r");
    if (fd == NULL)
    {
        perror("Error opening scene file!");
        return -1;
    }

    fseek(fd, 0, SEEK_END);
    size_t file_size = ftell(fd);
    fseek(fd, 0, SEEK_SET);

    if (file_size > SCENE_MAX_FILE_SIZE)
    {
        fclose(fd);
        printf("Scene file too large: %lu bytes\n", (unsigned long)file_size);
        return -1;
    }

    char *scene_text = malloc(file_size + 1);

    if (fread(scene_text, sizeof(char), file_size, fd) != file_size)
    {
        perror("Error reading scene file!");
        free(scene_text);
        return -1;
    }
    scene_text[file_size] = '\0';
    char *scene_line = scene_text;
    char *new_text = scene_text;
    // Parse line by line
    char *line_end;
    int done = 0;
    int line_num = 0;
    while (done == 0)
    {
        line_num++;
        line_end = strchr(new_text, '\n');

        if (line_end == NULL)
        {
            line_end = strchr(new_text, '\0');
            if (line_end == NULL)
            {
                printf("Error parsing: new line\n");
                goto error;
            }

            done = 1;
        }

        if (line_end == NULL)
        {
            break; // We done
        }
        scene_line = new_text;
        *line_end = '\0';    // Set 0 terminator
        new_text = line_end; // Next line
        new_text++;

        // printf("Line: %s\n", scene_line);

        // Chek what command
        scene_line = strtok(scene_line, ":");
        if (scene_line == NULL)
        {
            // No command on this line -> skip
            continue;
        }

        if (scene_line[0] == '/')
            continue; // This line is a comment

        // printf("CMD: %s\n", scene_line);
        int cmd = -1;
        for (int i = 0; i < SCENE_NUMBER_OF_COMMANDS; i++)
        {
            // Find which command is this
            if (strcmp(scene_line, commands[i]) == 0)
            {
                cmd = i;
                break;
            }
        }

        if (cmd == -1)
        {
            printf("%s:%d: no souch command: %s\n", filename, line_num, scene_line);
            continue; // Next line
        }
        // printf("CMD ID is: %d\n", cmd);
        switch (cmd)
        {
        case SCENE_CMD_ENABLE:
            cmd_enable(scene_line, filename, line_num);
            break;

        case SCENE_CMD_DISABLE:
            cmd_disable(scene_line, filename, line_num);
            break;

        case SCENE_CMD_WAIT:
            cmd_wait(scene_line, filename, line_num);
            break;

        case SCENE_CMD_LOAD:
            cmd_load(scene_line, filename, line_num);
            break;

        case SCENE_CMD_LOAD_SPRITE:
            cmd_sprite_load(scene_line, filename, line_num);
            break;
        case SCENE_CMD_ACTION:
            cmd_action(scene_line, filename, line_num);
            break;
        default:
            break;
        }
        // Some command is here
    }
    goto end;

error:
    free(scene_text);
    return -1;

end:
    free(scene_text);
    return 0;
}