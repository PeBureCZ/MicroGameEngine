#pragma once
#include <memory>
#include <unordered_map>
#include <deque>
#include "MlEvent.h"
#include <variant>
#include <optional>
#include <vector>
#include <mutex>
#include <string>
#include <string_view>

#include "BasicTypes.h"
#include "GraphicDependencies.h"
#include "layerDefinition.h"

namespace sf
{
    class Font;
	class View;
    class Sprite;
	class Text;
    class Texture;
    class RenderWindow;
}

struct MlVerticesObject;
class MgeImage;
class MgeText;

namespace ML_wrapper
{
    constexpr std::wstring_view DEFAULT_WINDOW_TITLE = L"Mge Application";

    class MlWrapper
    {
    public:
        MlWrapper(bool fullscreen = false, const std::wstring& appName = DEFAULT_WINDOW_TITLE.data());

        void pollMlEvents();
        void clear();
        void displayActualFrame();
        void drawContent();
        void resetMainWindow();

        void removeMgeLayerObject(const std::shared_ptr<MgeLayerObject> removedObject);
        void changeLayerOfMgeObject(std::shared_ptr<MgeLayerObject> mgeObject, size_t newLayer);

        [[nodiscard]] const FPoint& getCursorWorldPosition() const noexcept;
        [[nodiscard]] const IPoint& getCursorGuiPosition() const noexcept;
        void addMgeLayerObject(const std::shared_ptr<MgeLayerObject> newObject);

        [[nodiscard]] mgeType::Size<int> getRenderWindowSize() const noexcept;
        [[nodiscard]] std::shared_ptr<sf::RenderWindow> getRenderWindow() noexcept;

        void setWindowTitle(const std::wstring& newTitle);
        [[nodiscard]] const std::wstring& getWindowTitle() const noexcept;

        /**fixed client size, centered on the desktop, no resize border / maximize box; kept through fullscreen*/
        void lockWindowSize(const ISize& clientSize);
        void unlockWindowSize();
        [[nodiscard]] bool isWindowSizeLocked() const noexcept;

        /**ignored while the size is locked or in fullscreen*/
        void maximizeWindow();
        void minimizeWindow();
        /**back from maximized / minimized*/
        void restoreWindow();
        [[nodiscard]] bool isWindowMaximized() const noexcept;
        [[nodiscard]] bool isWindowMinimized() const noexcept;

        /**recreates the render window (desktop resolution / the last windowed size and position)*/
        void setFullscreen(bool isFullscreen);
        [[nodiscard]] bool isFullscreen() const noexcept;

		void moveScreenOffset(FPoint offset) noexcept;
		void setMaxScreenZoom(double zoomFactor) noexcept;
		void setMinScreenZoom(double zoomFactor) noexcept;
		void setZoom(double zoomFactor);
        [[nodiscard]] double getZoom() const noexcept;

        void setDrawToGuiView() const;
        void setDrawToWorldView() const;

        MlWrapper(const MlWrapper&) = delete;
        MlWrapper(MlWrapper&&) = delete;
        MlWrapper operator=(const MlWrapper&) = delete;
        MlWrapper operator=(MlWrapper&&) = delete;

        void sortLayerByGroupAndZ_delayed(size_t layer);

        const sf::Texture& appendTexture(const MgeImage& image);
        [[nodiscard]] mgeType::Size<int> getTextureSize(TextureId id);

        ~MlWrapper();

    private:

        void removeTexture(TextureId textureId);
        void doSortLayerIfNeeded(MgeLayer& layer);
        void createLayerIfNeeded(size_t layer);
        /**views follow the render window size*/
        void updateViewsToWindowSize();
        void applyWindowLock();

        template <typename Event>
        void sendEvent(Event&& ev)
        {
            getEventSystem().pushEvent(std::forward<Event>(ev));
        }

        std::mutex delayedSortMutex;
        std::vector<size_t> delayedSort; //used to store "do sort" during next app tick

		MGE_LAYERS layers;

        using TEXTURE_COUNT = size_t;
        using TEXTURE_OBJECT = std::pair<TEXTURE_COUNT, sf::Texture>;
        using TEXTURES = std::unordered_map<TextureId, TEXTURE_OBJECT>;
        TEXTURES textures;

        sf::View worldView;
        sf::View guiView;

        FPoint cursorWorldPosition;
        IPoint cursorGuiPosition;

        // Scale factor management - ensure correct scaling of ML elements on different resolutions / view sizes
        double zoom = 1.0;
		double maxZoom = 50.0;
		double minZoom = 0.01;

        bool m_isFullscreen = false;
        std::wstring m_windowTitle;
        std::optional<ISize> m_lockedClientSize_opt;
        //restored when leaving fullscreen
        ISize m_windowedSize;
        IPoint m_windowedPosition;
    };

    [[nodiscard]] bool initializeMlWrapper(bool fullscreen, const std::wstring& appName = DEFAULT_WINDOW_TITLE.data());
    std::shared_ptr<ML_wrapper::MlWrapper> getGlobalMlWrapper();
}

namespace ml
{
    [[nodiscard]] FPoint getCursorPosition() noexcept;
} //namespace ml end

                                                                      