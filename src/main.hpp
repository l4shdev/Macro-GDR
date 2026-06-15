#pragma once

#include <geode/mod.hpp>
#include <geode/ui/Popup.hpp>
#include <geode/loader/Dirs.hpp>
#include <vector>
#include <string>

struct MacroFrame {
    double timestamp;
    bool isTouchDown;
};

class MacroManager {
public:
    static MacroManager* get();

    void startRecording();
    void stopRecording();
    void startPlayback();
    void stopPlayback();
    
    void saveToFile(const std::string& filename);
    void loadFromFile(const std::string& filename);

    bool isRecording = false;
    bool isPlaying = false;
    std::vector<MacroFrame> frames;
    double startTime = 0.0;
    size_t currentPlaybackFrame = 0;
};

class MacroUI : public geode::Popup<std::string> {
public:
    bool setup(std::string filename) override;
    
    void onRecord(CCObject* sender);
    void onStop(CCObject* sender);
    void onPlay(CCObject* sender);
    void onSave(CCObject* sender);
    void onLoad(CCObject* sender);

    static MacroUI* create(const std::string& filename);
};
