#pragma once

#include "..\MeterWnd\MeterWnd.h"

class SliderKnob;

class SliderWnd : public MeterWnd {
protected:
    bool _dragging;
    bool _vertical;

    SliderWnd(LPCWSTR className, LPCWSTR title, HINSTANCE hInstance = NULL);

    virtual void Show();

    void Knob(SliderKnob *knob);
    virtual void SliderChanged() = 0;
    virtual void ScrollUp() = 0;
    virtual void ScrollDown() = 0;
    virtual void KeyPress(unsigned int vk) = 0;

private:
    bool _ignoreInput;
    SliderKnob *_knob;
    int _dragOffset;

    void PositionWindow();
    bool MouseOverKnob(int x, int y);
    bool MouseOverTrack(int x, int y);
    void UpdateKnob(int x, int y);

    virtual LRESULT WndProc(UINT message, WPARAM wParam, LPARAM lParam);
};