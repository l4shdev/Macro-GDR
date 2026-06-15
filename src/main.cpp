#include "main.hpp"
#include <geode/hooks.hpp>
#include <geode/cocos.hpp>
#include <geode/utils/file.hpp>
#include <sstream>
#include <iomanip>
#include <chrono>

using namespace geode::prelude;

double getCurrentTime() {
    auto now = std::chrono::high_resolution_clock::now();
    return std::chrono::duration<double>(now.time_since_epoch()).count();
}

MacroManager* MacroManager::get() {
    static MacroManager instance;
    return &instance;
}

void MacroManager::startRecording() {
    isRecording = true;
    isPlaying = false;
    frames.clear();
    startTime = getCurrentTime();
    log::info("Recording started!");
}

void MacroManager::stopRecording() {
    isRecording = false;
    log::info("Recording stopped. Total frames: {}", frames.size());
}

void MacroManager::startPlayback() {
    if (frames.empty()) {
        log::warn("No macro recorded to play!");
        return;
    }
    isPlaying = true;
    isRecording = false;
    currentPlaybackFrame = 0;
    startTime = getCurrentTime();
    log::info("Playback started!");
}

void MacroManager::stopPlayback() {
    isPlaying = false;
    log::info("Playback stopped.");
}

void MacroManager::saveToFile(const std::string& filename) {
    std::stringstream ss;
    for (const auto& frame : frames) {
        ss << frame.timestamp << "," << (frame.isTouchDown ? "1" : "0") << "\n";
    }
    
    auto path = getModConfigDir() / (filename + ".macro");
    if (file::writeString(path, ss.str())) {
        log::info("Saved macro to: {}", path.string());
    } else {
        log::error("Failed to save macro!");
    }
}

void MacroManager::loadFromFile(const std::string& filename) {
    auto path = getModConfigDir() / (filename + ".macro");
    auto result = file::readString(path);
    if (!result) {
        log::error("Failed to load macro: {}", result.unwrapErr());
        return;
    }

    frames.clear();
    std::stringstream ss(result.unwrap());
    std::string line;
    while (std::getline(ss, line)) {
        size_t pos = line.find(',');
        if (pos != std::string::npos) {
            MacroFrame frame;
            frame.timestamp = std::stod(line.substr(0, pos));
            frame.isTouchDown = (line.substr(pos + 1) == "1");
            frames.push_back(frame);
        }
    }
    log::info("Loaded {} frames from macro.", frames.size());
}

class $modify(TouchDispatcherHook, cocos2d::CCTouchDispatcher) {
    void touchesBegan(cocos2d::CCSet* touches, cocos2d::CCEvent* event) {
        $original(touchesBegan)(touches, event);
        
        auto* manager = MacroManager::get();
        if (manager->isRecording && PlayLayer::get()) {
            MacroFrame frame;
            frame.timestamp = PlayLayer::get()->m_levelTime;
            frame.isTouchDown = true;
            manager->frames.push_back(frame);
        }
    }

    void touchesEnded(cocos2d::CCSet* touches, cocos2d::CCEvent* event) {
        $original(touchesEnded)(touches, event);
        
        auto* manager = MacroManager::get();
        if (manager->isRecording && PlayLayer::get()) {
            MacroFrame frame;
            frame.timestamp = PlayLayer::get()->m_levelTime;
            frame.isTouchDown = false;
            manager->frames.push_back(frame);
        }
    }
};

class $modify(PlayLayerHook, PlayLayer) {
    void update(float dt) override {
        $original(update)(dt);

        auto* manager = MacroManager::get();
        if (manager->isPlaying && !this->m_isPaused) {
            double currentTime = this->m_levelTime;
            
            while (manager->currentPlaybackFrame < manager->frames.size()) {
                auto& frame = manager->frames[manager->currentPlaybackFrame];
                
                if (currentTime >= frame.timestamp - 0.01) {
                    if (this->m_player1) {
                        if (frame.isTouchDown) {
                            this->m_player1->pushButton(PlayerButton::Jump);
                        } else {
                            this->m_player1->releaseButton(PlayerButton::Jump);
                        }
                    }
                    manager->currentPlaybackFrame++;
                } else {
                    break;
                }
            }
        }
    }
};

bool MacroUI::setup(std::string filename) {
    this->setTitle("Macro Recorder");
    this->m_noElasticity = true;

    auto layout = ColumnLayout::create();
    layout->setGap(10);
    this->m_mainLayer->setLayout(layout);

    auto input = TextInput::create(200, "File Name (e.g., my_macro)");
    input->setString(filename.empty() ? "macro1" : filename);
    input->setID("filename_input");
    this->m_mainLayer->addChild(input);

    auto btnRecord = CCMenuItemSpriteExtra::create(
        ButtonSprite::create("Record", "goldFont.fnt", "GJ_button_01.png"),
        this, menu_selector(MacroUI::onRecord)
    );
    this->m_mainLayer->addChild(btnRecord);

    auto btnStop = CCMenuItemSpriteExtra::create(
        ButtonSprite::create("Stop", "goldFont.fnt", "GJ_button_01.png"),
        this, menu_selector(MacroUI::onStop)
    );
    this->m_mainLayer->addChild(btnStop);

    auto btnPlay = CCMenuItemSpriteExtra::create(
        ButtonSprite::create("Play", "goldFont.fnt", "GJ_button_01.png"),
        this, menu_selector(MacroUI::onPlay)
    );
    this->m_mainLayer->addChild(btnPlay);

    auto btnSave = CCMenuItemSpriteExtra::create(
        ButtonSprite::create("Save File", "goldFont.fnt", "GJ_button_04.png"),
        this, menu_selector(MacroUI::onSave)
    );
    this->m_mainLayer->addChild(btnSave);

    auto btnLoad = CCMenuItemSpriteExtra::create(
        ButtonSprite::create("Load File", "goldFont.fnt", "GJ_button_04.png"),
        this, menu_selector(MacroUI::onLoad)
    );
    this->m_mainLayer->addChild(btnLoad);

    this->m_mainLayer->updateLayout();
    return true;
}

void MacroUI::onRecord(CCObject*) {
    MacroManager::get()->startRecording();
    Notification::create("Recording Started!", NotificationIcon::Success)->show();
}

void MacroUI::onStop(CCObject*) {
    MacroManager::get()->stopRecording();
    MacroManager::get()->stopPlayback();
    Notification::create("Stopped", NotificationIcon::Info)->show();
}

void MacroUI::onPlay(CCObject*) {
    MacroManager::get()->startPlayback();
    Notification::create("Playback Started!", NotificationIcon::Success)->show();
}

void MacroUI::onSave(CCObject*) {
    auto input = this->getChildByID("filename_input");
    if (auto textInput = typeinfo_cast<TextInput*>(input)) {
        MacroManager::get()->saveToFile(textInput->getString());
        Notification::create("Saved!", NotificationIcon::Success)->show();
    }
}

void MacroUI::onLoad(CCObject*) {
    auto input = this->getChildByID("filename_input");
    if (auto textInput = typeinfo_cast<TextInput*>(input)) {
        MacroManager::get()->loadFromFile(textInput->getString());
        Notification::create("Loaded!", NotificationIcon::Success)->show();
    }
}

MacroUI* MacroUI::create(const std::string& filename) {
    auto ret = new MacroUI();
    if (ret && ret->initAnchored(300, 250, filename)) {
        ret->autorelease();
        return ret;
    }
    delete ret;
    return nullptr;
}

class $modify(MenuLayerHook, MenuLayer) {
    bool init() override {
        if (!$original(init)()) return false;

        auto btnMacro = CCMenuItemSpriteExtra::create(
            ButtonSprite::create("Macro", "bigFont.fnt", "GJ_button_01.png"),
            this,
            menu_selector(MenuLayerHook::onOpenMacroUI)
        );
        
        if (auto menu = this->getChildByID("bottom-menu")) {
            menu->addChild(btnMacro);
            btnMacro->setPosition(ccp(100, 50)); 
        }

        return true;
    }

    void onOpenMacroUI(CCObject*) {
        MacroUI::create("")->show();
    }
};
