#pragma once

enum ScreenGameEvent : int
{
    Undefined = -1,

    //main menu
    StartGame,
    OpenSettings,
    OpenEditor,
    LoadGame,
    ExitGame,

    //editor
    ExitEditor,
};
