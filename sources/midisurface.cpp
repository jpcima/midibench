#include "midisurface.h"
#include "widgets.h"
#include <imgui.h>
#include <rtmidi/RtMidi.h>
#include <cstdio>

MidiSurface::MidiSurface()
    : midi_port_(new RtMidiOut(RtMidi::UNSPECIFIED, application_name()))
{
    midi_port_->openVirtualPort("Output");
}

MidiSurface::~MidiSurface()
{
}

const char* MidiSurface::application_name()
{
    return "MIDI surface";
}

const char* MidiSurface::window_title()
{
    return application_name();
}

int MidiSurface::window_width()
{
    return 1280;
}

int MidiSurface::window_height()
{
    return 720;
}

void MidiSurface::exec()
{
    const ImVec4 selection_color(1.0f, 0.9f, 0.0f, 1.0f);

    ///
    ImGui::SetNextWindowPos(ImVec2(10, 10));
    ImGui::SetNextWindowSize(ImVec2(1100, 445));
    ImGui::Begin("Controllers");

    ImGui::Columns(4);
    ImGui::Text("0 - 31");
    ImGui::NextColumn();
    ImGui::Text("32 - 63");
    ImGui::NextColumn();
    ImGui::Text("64 - 95");
    ImGui::NextColumn();
    ImGui::Text("96 - 127");
    ImGui::NextColumn();
    ImGui::Separator();
    for (int i = 0; i < 128; ++i) {
        int cc = (i % 4) * 32 + (i / 4) % 32;
        char name[128];
        sprintf(name, "CC %d\n%s", cc, controller_name(cc));
        if (ImGui::SliderInt(name, &controllers_[cc], 0, 127))
            write_midi3(0xb0|channel_, cc, controllers_[cc]);
        ImGui::NextColumn();
    }

    ImGui::End();

    ///
    ImGui::SetNextWindowPos(ImVec2(1120, 10));
    ImGui::SetNextWindowSize(ImVec2(150, 445));
    ImGui::Begin("Status");

    ImGui::Text("Message");
    for (int i = 0, n = last_message_size_; i < n; ++i) {
        if (i > 0)
            ImGui::SameLine();
        ImGui::TextColored(selection_color, "%02X", last_message_[i]);
    }
    for (int i = last_message_size_; i < 3; ++i) {
        if (i > 0)
            ImGui::SameLine();
        ImGui::TextColored(selection_color, "--");
    }

    ImGui::End();

    ///
    ImGui::SetNextWindowPos(ImVec2(10, 465));
    ImGui::SetNextWindowSize(ImVec2(920, 247));
    ImGui::Begin("Keys");

    const char* key_names[12] = {
        "C", "C#", "D", "D#", "E",
        "F", "F#", "G", "G#",  "A", "A#", "B",
    };

    ImGui::Columns(2);
    int selected_key = selected_key_;
    for (int i = 0; i < 128; ++i) {
        if (i > 0 && i % 12 == 0)
            ImGui::NextColumn();
        else if (i > 0)
            ImGui::SameLine();
        char name[12] = {};
        sprintf(name, "%3d\n%s", i, key_names[i % 12]);

        if (i == selected_key) {
            ImGui::PushStyleColor(ImGuiCol_Border, selection_color);
            ImGui::PushStyleVar(ImGuiStyleVar_FrameBorderSize, 1);
        }

        bool key_interact;
        if (hold_keys_pressed_)
            key_interact = ToggleButton(name, &keypressed_[i]);
        else
            key_interact = KickButton(name, &keypressed_[i]);

        if (key_interact) {
            if (keypressed_[i])
                write_midi3(0x90|channel_, i, key_on_velocity_);
            else if (key_off_velocity_ == 0)
                write_midi3(0x90|channel_, i, 0);
            else
                write_midi3(0x80|channel_, i, key_off_velocity_);
            selected_key_ = i;
        }

        if (i == selected_key) {
            ImGui::PopStyleColor();
            ImGui::PopStyleVar();
        }
    }

    ImGui::NextColumn();
    ImGui::Spacing();
    ImGui::Spacing();
    if (ImGui::Checkbox("Hold", &hold_keys_pressed_)) {
        if (!hold_keys_pressed_) {
            for (int i = 0; i < 128; ++i)
                keypressed_[i] = false;
        }
    }

    ImGui::End();

    ///
    ImGui::SetNextWindowPos(ImVec2(940, 465));
    ImGui::SetNextWindowSize(ImVec2(330, 247));
    ImGui::Begin("Other");

    ++channel_;
    ImGui::SliderInt("Channel", &channel_, 1, 16);
    --channel_;
    ImGui::SliderInt("Velocity on", &key_on_velocity_, 0, 127);
    ImGui::SliderInt("Velocity off", &key_off_velocity_, 0, 127);
    ImGui::Separator();
    if (ImGui::SliderInt("Program", &program_, 0, 127))
        write_midi2(0xc0|channel_, program_);
    if (ImGui::SliderInt("Bend", &bend_, -8192, 8191)) {
        int value = bend_ + 8192;
        write_midi3(0xe0|channel_, value & 0x7f, value >> 7);
    }
    ImGui::Separator();
    ImGui::Text("Current key:");
    ImGui::SameLine();
    ImGui::TextColored(selection_color, "%d", selected_key_);
    if (ImGui::SliderInt("Key Aftertouch", &aftertouch_, 0, 127)) {
        if (selected_key_ != -1)
            write_midi3(0xa0|channel_, selected_key_, aftertouch_);
    }
    if (ImGui::SliderInt("Ch Aftertouch", &channel_aftertouch_, 0, 127))
        write_midi2(0xd0|channel_, channel_aftertouch_);

    ImGui::End();
}

void MidiSurface::write_midi(const uint8_t* data, uint32_t size)
{
    midi_port_->sendMessage(data, size);

    for (uint32_t i = 0; i < size && i < sizeof(last_message_); ++i)
        last_message_[i] = data[i];
    for (uint32_t i = size; i < sizeof(last_message_); ++i)
        last_message_[i] = 0;
    last_message_size_ = size;
}

void MidiSurface::write_midi2(uint8_t status, uint8_t data)
{
    uint8_t buf[] = {status, data};
    write_midi(buf, 2);
}

void MidiSurface::write_midi3(uint8_t status, uint8_t data1, uint8_t data2)
{
    uint8_t buf[] = {status, data1, data2};
    write_midi(buf, 3);
}

const char* MidiSurface::controller_name(int cc)
{
    switch (cc) {
    case 0:          return "Bank Sel";
    case 1:          return "Mod Wheel";
    case 2:          return "Breath Con";
    case 4:          return "Foot Con";
    case 5:          return "Porta Time";
    case 6:          return "Data MSB";
    case 7:          return "Ch Volume";
    case 8:          return "Balance";
    case 10:         return "Pan";
    case 11:         return "Expression";
    case 12:         return "Fx 1";
    case 13:         return "Fx 2";
    case 16:         return "Gen 1";
    case 17:         return "Gen 2";
    case 18:         return "Gen 3";
    case 19:         return "Gen 4";
    case 32:         return "LSB 0";
    case 33:         return "LSB 1";
    case 34:         return "LSB 2";
    case 35:         return "LSB 3";
    case 36:         return "LSB 4";
    case 37:         return "LSB 5";
    case 38:         return "LSB 6";
    case 39:         return "LSB 7";
    case 40:         return "LSB 8";
    case 41:         return "LSB 9";
    case 42:         return "LSB 10";
    case 43:         return "LSB 11";
    case 44:         return "LSB 12";
    case 45:         return "LSB 13";
    case 46:         return "LSB 14";
    case 47:         return "LSB 15";
    case 48:         return "LSB 16";
    case 49:         return "LSB 17";
    case 50:         return "LSB 18";
    case 51:         return "LSB 19";
    case 52:         return "LSB 20";
    case 53:         return "LSB 21";
    case 54:         return "LSB 22";
    case 55:         return "LSB 23";
    case 56:         return "LSB 24";
    case 57:         return "LSB 25";
    case 58:         return "LSB 26";
    case 59:         return "LSB 27";
    case 60:         return "LSB 28";
    case 61:         return "LSB 29";
    case 62:         return "LSB 30";
    case 63:         return "LSB 31";
    case 64:         return "Damper Sw";
    case 65:         return "Porta Sw";
    case 66:         return "Sost Sw";
    case 67:         return "Soft Sw";
    case 68:         return "Legato Sw";
    case 69:         return "Hold 2";
    case 70:         return "Sound 1";
    case 71:         return "Sound 2";
    case 72:         return "Sound 3";
    case 73:         return "Sound 4";
    case 74:         return "Sound 5";
    case 75:         return "Sound 6";
    case 76:         return "Sound 7";
    case 77:         return "Sound 8";
    case 78:         return "Sound 9";
    case 79:         return "Sound 10";
    case 80:         return "General 5";
    case 81:         return "General 6";
    case 82:         return "General 7";
    case 83:         return "General 8";
    case 84:         return "Porta Con";
    case 88:         return "Vel Prefix";
    case 91:         return "Fx 1 Depth";
    case 92:         return "Fx 2 Depth";
    case 93:         return "Fx 3 Depth";
    case 94:         return "Fx 4 Depth";
    case 95:         return "Fx 5 Depth";
    case 96:         return "Data +1";
    case 97:         return "Data -1";
    case 98:         return "NRPN LSB";
    case 99:         return "NRPN MSB";
    case 100:        return "RPN LSB";
    case 101:        return "RPN MSB";
    case 120:        return "Sound Off";
    case 121:        return "Reset All";
    case 122:        return "Local Con";
    case 123:        return "Notes Off";
    case 124:        return "Omni Off";
    case 125:        return "Omni On";
    case 126:        return "Mono On";
    case 127:        return "Poly On";
    default:         return "-";
    }
}
