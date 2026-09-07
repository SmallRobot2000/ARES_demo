Tutorial for .sen format:

`//` or `/` -> comment only on start of the line, 

`enable:<layer: B0/T0/T1/S0>` -> enables selected layer

`disable:<layer: B0/T0/T1/S0>` -> disable selected layer

`action:<layer: B0/T0/T1/S0>:<action>` -> does action on selected buffer

`load:<layer: B0/T0/T1>:<filename>:<x_off>:<y_off>` -> loads "filename.<extension for correct layer> to that layer data/palette/map and offset"

`load_sprite:<layer: S0>:<filename>:<offset>:<sprite_count>:<sprite size 16 or 32>:<pal_num>` -> loads `<filename>` to S0 with specified parameters




`wait:<amaunt>` -> waits `<amaunt>` in ms


Types of `<action>`:

For layers `B0/T0/T1`:

`scroll_x_set:<amaunt>` -> sets that layers x offset to `<amaunt>`

`scroll_y_set:<amaunt>` -> sets that layers y offset to `<amaunt>`

`scroll_x_delta:<x_delta>` -> adds `x_delta` (which can be negative) to that layers x offset

`scroll_y_delta:<y_delta>` -> adds `y_delta` (which can be negative) to that layers y offset

Only for `S0` layer:

`sprite_activate:<sprite number>` -> activate that sprite `<sprite number>`(0-63)

`sprite_deactivate:<sprite number>` -> activate that sprite `<sprite number>`(0-63)

`sprite_set:<sprite number>:<x_pos>:<y_pos>:<h_flip 1 or 0>:<v_flip 1 or 0>:<size(16 or 32)>:<scale(0 - 3)>` -> sets all basic sprite attributes to sprite of sprite_number

`sprite_move:<sprite number>:<x_delta>:<y_delta>` -> moves sprite `<sprite_number>` by `x_delta` and `y_delta`

`sprite_h_flip:<sprite number>:<h_flip 1 or 0>` -> sets `<sprite_number>` `h_flip` attribute

`sprite_v_flip:<sprite number>:<v_flip 1 or 0>` -> sets `<sprite_number>` `v_flip` attribute

`sprite_scale:<sprite number>:<scale (0 - 3)>` -> sets `<sprite_number>` `scale` attribute



# Example:

Loads layer B0 and S0 with data, Initializes Sprite 0 as 32x32 scale 2 sprite and moves it

```
enable:B0
enable:S0
load:B0:bitmap1.b0
//Makes big 32x32 sprite sprite
load_sprite:S0:sprites.spr:0:8:32:0
action:S0:sprite_set:0:45:54:0:0:32:2 
action:S0:sprite_activate:0
//Wait 1 second
wait:1000 
action:S0:sprite_move:0:10:100
```
