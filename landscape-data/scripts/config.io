Config do(
  old_setSlot := getSlot("setSlot")
  
  forward := method(
    query(thisMessage name)
  )
  setSlot:=method(key, value,
    if (hasSlot(key), old_setSlot(key,value), set(key,value))
    value
  )

  bin_dir     := base_dir + "/bin"
  data_dir    := base_dir + "/share/landscape"
  terrain_dir := data_dir + "/lod-terrains/compare"
  texture_dir := data_dir + "/textures"
  model_dir   := data_dir + "/models"
  fonts_dir   := data_dir + "/fonts"
  sounds_dir  := data_dir + "/sounds"
  scripts_dir := data_dir + "/scripts"
  
  // Game config
  Game_windowed := "1"
  if(Game_windowed != "0") then(
    Game_grab_mouse := "false"
    Game_xres       := "800"
    Game_yres       := "600"
    Game_fullscreen := "0"
  ) else (
    Game_grab_mouse := "true"
    Game_xres       := "1024"
    Game_yres       := "768"
    Game_fullscreen := "1"
  )
  
  Game_loading_screen := texture_dir+"/loading-screen.spr"

  // Sound config
  SoundMan_sound_dir        := sounds_dir
  SoundMan_doppler_velocity := "5000"
  SoundMan_doppler_factor   := "1"
  SoundMan_channels         := "64"

  // Level of Detail Terrain Manager configuration
  LoDQuadManager_terrain_prefix    := terrain_dir+"/terrain"
  LoDQuadManager_texture_prefix    := terrain_dir+"/texture"
  LoDQuadManager_texmap_prefix     := terrain_dir+"/texmap"
  LoDQuadManager_lightmap_prefix   := terrain_dir+"/lightmap"
  LoDQuadManager_quads_w           := "1"
  LoDQuadManager_quads_h           := "1"
  LoDQuadManager_detail_tex_name   := texture_dir+"/detail-texture.spr"
  LoDQuadManager_tile_dir          := texture_dir+"/terrain-tiles"
  LoDQuadManager_texture_list      := terrain_dir+"/textures"

  // Map configuration
  Map_texture_file                 := terrain_dir+"/map.spr"

  // Camera configuration
  Camera_focus                     := "1.5"
  Camera_aspect                    := "1.33333"

  // Sky (clouds layer) configuration
  Sky_upside_texture_path          := texture_dir+"/sky-layer1-above.spr"
  Sky_downside_texture_path        := texture_dir+"/sky-layer1-below.spr"

  // SkyBox configuration
  SkyBox_texture_path              := texture_dir+"/skybox/pastel"

  // Gunsight configuration
  Gunsight_texture_file            := texture_dir+"/gunsight.spr"

  // SmokeTrail configuration
  SmokeTrail_smoke_tex             := texture_dir+"/missile_smoke.spr"
  SmokeTrail_puffy_tex             := texture_dir+"/puffy_smoke.spr"

  // SmokeColumn configuration
  SmokeColumn_puffy_tex            := texture_dir+"/puffy_smoke.spr"

  // Explosion configuration
  Explosion_frames                 := "16"
  Explosion_seconds_per_frame      := "0.08"
  Explosion_filename_prefix        := texture_dir+"/explosion/explosion"
  Explosion_filename_postfix       := ".png"
  Explosion_size                   := "20.0"
  Explosion_sound                  := "explosion-01.wav"

  // Player configuration
  Player_model_path                := model_dir+"/thunder"
  Player_model_file                := Player_model_path+"/thunder.obj"
  Player_model_bounds              := Player_model_path+"/thunder.bounds"

  // Drone configuration
  //Drone_model_path                 := model_dir+"/thunder"
  //Drone_inside_model_file          := Drone_model_path+"/thunder.obj"
  //Drone_outside_model_file         := Drone_model_path+"/thunder.obj"
  //Drone_model_bounds               := Drone_model_path+"/thunder.bounds"
  
  Drone_model_path                 := model_dir+"/lightning"
  Drone_inside_model_file          := Drone_model_path+"/lightning-with-interior.obj"
  Drone_outside_model_file         := Drone_model_path+"/lightning.obj"
  Drone_model_bounds               := Drone_model_path+"/lightning.bounds"
  
  Drone_target_radius              := "6.5"
  Drone_pilot_pos                  := "Vector3( 0 0.8 1.9)"
  Drone_cannon_sound               := "cannon-shoot-1.wav"
  Drone_engine_sound               := "aircraft-engine-1.wav"
  Drone_engine_gain                := "0.1"
  Drone_wheel_model_file           := model_dir+"/misc/tire.obj"

  // Tank configuration
  Tank_model_path                  := model_dir+"/tank"
  Tank_base_file                   := Tank_model_path+"/base.obj"
  Tank_tower_file                  := Tank_model_path+"/tower.obj"
  Tank_cannon_file                 := Tank_model_path+"/cannon.obj"
  Tank_cannon_sound                := "cannon-shoot-1.wav"
  Tank_engine_sound_low            := "car-engine-1-loop.wav"
  Tank_engine_sound_high           := "car-engine-2-loop.wav"

  // FontMan configuration
  FontMan_dir                      := fonts_dir

  // Io Scripting configuration
  Io_init_script                   := scripts_dir+"/init.io"
  Io_init_script_2                 := scripts_dir+"/init2.io"
  
  DumbMissile_model                := model_dir+"/missiles/dumbmissile.obj"
  SmartMissile_model               := model_dir+"/missiles/smartmissile.obj"
  Missile_engine_sound             := "rocket-engine-1.wav"
  Missile_Ka                       := "20"
  Missile_Kb                       := "0.1"
  Missile_Kc                       := "0"

) // Config do
