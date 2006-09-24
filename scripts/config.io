Config do(
  old_setSlot := getSlot("setSlot")
  
  // This definition makes Config call query(key) for every slot not found in Config
  forward := method(
    query(call message name)
  )
  
  // The following line change the behavior of "setSlot" and "setSlotWithType"
  // to call the set(key, value) method.
  // This is very tricky, don't modify if you don't have to!
  configSetSlot := method(key, value,
    "In setSlot!" println
    if (hasSlot(key), old_setSlot(key,value), set(key,value))
    write("Config: " .. key .. "=" .. value .. "\n")
    value
  )
  setSlotWithType := getSlot("configSetSlot")
  setSlot := getSlot("configSetSlot")
  
  // -------------------------------------------------------------------------
  // ------- After this point, all assignments are configuration data! -------
  // -------------------------------------------------------------------------
  
  bin_dir     := base_dir .. "/bin"
  data_dir    := base_dir .. "/share/landscape"
  terrain_dir := data_dir .. "/lod-terrains/compare"
  texture_dir := data_dir .. "/textures"
  model_dir   := data_dir .. "/models"
  fonts_dir   := data_dir .. "/fonts"
  sounds_dir  := data_dir .. "/sounds"
  scripts_dir := data_dir .. "/scripts"
  
  
  // Game config
  Game_enable_SDL_parachute := "false"
  Screen_mode := "0"
  Game_grab_mouse := if(Screen_mode == "0", "false", "true")
  Game_fullscreen := if(Screen_mode == "0", "false", "true")
  if(Screen_mode == "0") then(
    Game_xres       := "800"
    Game_yres       := "600"
  ) elseif (Screen_mode == "1") then (
    Game_xres       := "800"
    Game_yres       := "600"
  ) elseif (Screen_mode == "2") then (
    Game_xres       := "1024"
    Game_yres       := "768"
  ) elseif (Screen_mode == "3") then (
    Game_xres       := "1280"
    Game_yres       := "1024"
  ) elseif (Screen_mode == "4") then (
    Game_xres       := "1400"
    Game_yres       := "1050"
  ) elseif (Screen_mode == "5") then (
    Game_xres       := "1600"
    Game_yres       := "1200"
  ) else (
    Exception raise("Unknown screen mode" , "Encountered unknown screen mode " .. Screen_mode .. " while reading configuration")
  )
  
  Game_red_bits     := "5"
  Game_green_bits   := "5"
  Game_blue_bits    := "5"
  Game_zbuffer_bits := "1"
  
  Game_loading_screen := texture_dir .. "/loading-screen.spr"
  
  // Controls config
  Controls_enable_joystick := "false"
  Controls_enable_mouse    := "true"

  // Sound config
  SoundMan_sound_dir        := sounds_dir
  SoundMan_doppler_velocity := "5000"
  SoundMan_doppler_factor   := "1"
  SoundMan_channels         := "64"
  SoundMan_minimum_gain     := "0.001"
  SoundMan_hysteresis       := "0.01"

  // Level of Detail Terrain Manager configuration
  LoDQuadManager_terrain_prefix    := terrain_dir .. "/terrain"
  LoDQuadManager_texture_prefix    := terrain_dir .. "/texture"
  LoDQuadManager_texmap_prefix     := terrain_dir .. "/texmap"
  LoDQuadManager_lightmap_prefix   := terrain_dir .. "/lightmap"
  LoDQuadManager_quads_w           := "1"
  LoDQuadManager_quads_h           := "1"
  LoDQuadManager_detail_tex_name   := texture_dir .. "/detail-texture.spr"
  LoDQuadManager_tile_dir          := texture_dir .. "/terrain-tiles"
  LoDQuadManager_texture_list      := terrain_dir .. "/textures"

  // Map configuration
  Map_texture_file                 := terrain_dir .. "/map.spr"

  // Camera configuration
  Camera_focus                     := "1.5"
  Camera_aspect                    := ((Game_xres asNumber / Game_yres asNumber) asString)

  // Sky (clouds layer) configuration
  Sky_upside_texture_path          := texture_dir .. "/sky-layer1-above.spr"
  Sky_downside_texture_path        := texture_dir .. "/sky-layer1-below.spr"

  // SkyBox configuration
  SkyBox_texture_path              := texture_dir .. "/skybox/pastel"

  // Gunsight configuration
  Gunsight_texture_file            := texture_dir .. "/gunsight.spr"

  // SmokeTrail configuration
  SmokeTrail_smoke_tex             := texture_dir .. "/missile_smoke.spr"
  SmokeTrail_puffy_tex             := texture_dir .. "/puffy_smoke.spr"

  // SmokeColumn configuration
  SmokeColumn_puffy_tex            := texture_dir .. "/puffy_smoke.spr"

  // Explosion configuration
  Explosion_frames                 := "16"
  Explosion_seconds_per_frame      := "0.08"
  Explosion_filename_prefix        := texture_dir .. "/explosion/explosion"
  Explosion_filename_postfix       := ".png"
  Explosion_size                   := "20.0"
  Explosion_sound                  := "explosion-01.wav"

  // Player configuration
  Player_model_path                := model_dir .. "/thunder"
  Player_model_file                := Player_model_path .. "/thunder.obj"
  Player_model_bounds              := Player_model_path .. "/thunder.bounds"

  // Drone configuration
  //Drone_model_path                 := model_dir .. "/thunder"
  //Drone_inside_model_file          := Drone_model_path .. "/thunder.obj"
  //Drone_outside_model_file         := Drone_model_path .. "/thunder.obj"
  //Drone_model_bounds               := Drone_model_path .. "/thunder.bounds"
  
  Drone_model_path                 := model_dir .. "/lightning"
  Drone_skeleton                   := Drone_model_path .. "/lightning.spec"
  Drone_inside_model_file          := Drone_model_path .. "/lightning-with-interior.obj"
  Drone_outside_model_file         := Drone_model_path .. "/lightning.obj"
  Drone_model_bounds               := Drone_model_path .. "/lightning.bounds"
  
  Drone_target_radius              := "3.5"
  Drone_pilot_pos                  := "Vector3( 0 0.8 1.9)"
  Drone_cannon_sound               := "cannon-shoot-1.wav"
  Drone_engine_sound               := "aircraft-engine-1.wav"
  Drone_engine_gain                := "0.1"
  Drone_wheel_model_file           := model_dir .. "/misc/tire.obj"

  // Tank configuration
  Tank_skeleton                    := model_dir .. "/newtank/Tank.spec"
  Tank_cannon_sound                := "cannon-shoot-1.wav"
  Tank_engine_sound_low            := "car-engine-1-loop.wav"
  Tank_engine_sound_high           := "car-engine-2-loop.wav"

  // FontMan configuration
  FontMan_dir                      := fonts_dir

  // Io Scripting configuration
  Io_init_script                   := scripts_dir .. "/init.io"
  Io_init_script_2                 := scripts_dir .. "/init2.io"
  
  DumbMissile_model                := model_dir .. "/missiles/dumbmissile.obj"
  SmartMissile_model               := model_dir .. "/missiles/smartmissile.obj"
  Missile_engine_sound             := "rocket-engine-1.wav"
  Missile_Ka                       := "20"
  Missile_Kb                       := "0.1"
  Missile_Kc                       := "0"
  
  // Weapons configuration
  Cannon_sound                     := "cannon-shoot-1.wav"
  Cannon_muzzle_velocity           := "1200"

) // Config do

