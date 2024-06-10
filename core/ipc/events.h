#pragma once

enum class IpcEvent {
  /* Shared */
  SHARED_HANDSHAKE, // <-->

  /* Gamereport */

  /* Emulator */
  EMU_LOAD_GAME,   // <--
  EMU_ADD_ARGS,    // <--
  EMU_RUN_GAME,    // <--
  EMU_EXEC_REQUEST // -->
};
