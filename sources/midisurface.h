#pragma once
#include <memory>
#include <cstdint>
class RtMidiOut;

class MidiSurface {
public:
    MidiSurface();
    ~MidiSurface();

    static const char* application_name();
    static const char* window_title();

    static int window_width();
    static int window_height();

    void exec();

    void write_midi(const uint8_t* data, uint32_t size);
    void write_midi2(uint8_t status, uint8_t data);
    void write_midi3(uint8_t status, uint8_t data1, uint8_t data2);

    static const char* controller_name(int cc);

private:
    std::unique_ptr<RtMidiOut> midi_port_;
    int channel_ {};
    int program_ {};
    int bend_ {};
    int aftertouch_ {};
    int channel_aftertouch_ {};
    int key_on_velocity_ {127};
    int key_off_velocity_ {};
    int last_pressed_key_ {-1};
    uint8_t last_message_[3] {};
    int last_message_size_ = 0;
    int controllers_[128] {};
    bool keypressed_[128] {};
};
