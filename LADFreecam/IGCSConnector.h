#pragma once
#define EXPOSE extern "C" __declspec(dllexport)

#include <cstdint>
#include <iostream>
#include <Windows.h>

struct Igcs;

Igcs* igcs_connector;



struct Igcs {
  
  Igcs() : camera_data(nullptr), igcs_lib(0), 
    movement{ 0.0 }, rendering(false) {
    igcs_lib = LoadLibraryA("IgcsConnector.addon64");
    if (!igcs_lib) {
      std::cout << "IgcsConnector.addon64 not found, not going to use IGCS DoF." << std::endl;
    }

    using GetDataFunc = std::uint8_t*(*)();
    auto enable_igcs_func = GetProcAddress(igcs_lib, "connectFromCameraTools");
    auto get_data_from_tools = reinterpret_cast<GetDataFunc>(GetProcAddress(igcs_lib, "getDataFromCameraToolsBuffer"));

    enable_igcs_func();
    camera_data = get_data_from_tools();

    std::cout << "IgcsConnector was loaded and enabled!" << std::endl;
  }

  bool check_enabled() {
    return igcs_lib != 0 && this->rendering;
  }

  void update_camera_status(bool enabled) {
    camera_data[0] = enabled ? 1 : 0;
  }

  void reset_deltas() {
    this->movement.delta_pos_x = 0.0;
    this->movement.delta_pos_z = 0.0;
  }

  struct {
    float delta_pos_x;
    float delta_pos_z;
  } movement;

  camera_info camera;
  bool rendering;

  // If we get a new delta
  bool pending_update = false;
private:
  std::uint8_t* camera_data;
  HMODULE igcs_lib;
  
};


EXPOSE int __cdecl IGCS_StartScreenshotSession(std::uint8_t _ignored) {
  if (!igcs_connector) { return 0; }
  std::cout << "Starting screenshot session" << std::endl;
  igcs_connector->rendering = true;
  igcs_connector->pending_update = true;
  return 0;
}

EXPOSE void __cdecl IGCS_EndScreenshotSession() {
  if (!igcs_connector) { return; }

  std::cout << "Ending screenshot session" << std::endl;
  igcs_connector->rendering = false;
  igcs_connector->reset_deltas();
  igcs_connector->pending_update = true;
}

// Unimplemented intentionally.
EXPOSE void __cdecl IGCS_MoveCameraPanorama() {}

EXPOSE void __cdecl IGCS_MoveCameraMultishot(float step_left, float step_up, float fov, int from_start) {
  if (!igcs_connector) { return; }
  printf("Commanded %f %f\n", step_left, step_up);
  igcs_connector->movement.delta_pos_x = -step_left;
  igcs_connector->movement.delta_pos_z = step_up;
  igcs_connector->pending_update = true;
}

