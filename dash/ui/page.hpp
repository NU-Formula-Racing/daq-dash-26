#ifndef __PAGE_H__
#define __PAGE_H__

#include "okay/core/renderer/renderer.hpp"
#include "okay/core/tween/tween_easing.hpp"

#include <okay/okay.hpp>

#include <functional>
#include <memory>
#include <thread>
#include <utility>
#include <vector>

#define BIND_TO_THIS(fnName)   \
    [this]() {                 \
        return this->fnName(); \
    }

#define LAMBDA_WRAP(fn) \
    []() {              \
        return fn();    \
    }

namespace dash {

class IPage {
   public:
    virtual ~IPage() = default;

    virtual void initializePage() = 0;
    virtual void closePage() = 0;
};

struct PageEntry {
    std::unique_ptr<IPage> page{nullptr};

    std::function<bool()> activeWhenPred{[]() {
        return true;
    }};

    std::function<bool()> forceOverrideWhenPred{[]() {
        return false;
    }};

    std::size_t priority{0};
    okay::Option<std::size_t> number;

    PageEntry() = default;

    explicit PageEntry(std::unique_ptr<IPage> newPage) : page(std::move(newPage)) {}

    static PageEntry create(std::unique_ptr<IPage> page) {
        return PageEntry(std::move(page));
    }

    PageEntry& activeWhen(std::function<bool()> fn) {
        activeWhenPred = std::move(fn);
        return *this;
    }

    PageEntry& forceOverrideWhen(std::function<bool()> fn) {
        forceOverrideWhenPred = std::move(fn);
        return *this;
    }

    PageEntry& withPriority(std::size_t newPriority) {
        priority = newPriority;
        return *this;
    }

    PageEntry& withPageNumber(std::size_t num) {
        number = num;
        return *this;
    }

    PageEntry(PageEntry&&) noexcept = default;
    PageEntry& operator=(PageEntry&&) noexcept = default;

    PageEntry(const PageEntry&) = delete;
    PageEntry& operator=(const PageEntry&) = delete;
};

class PageManager : public okay::System<okay::SystemScope::GAME> {
   public:
    template <typename... Pages>
    explicit PageManager(Pages&&... pages) {
        (_pages.emplace_back(std::move(pages)), ...);
        recountNumberedPages();
    }

    void initialize() {
        _currentPage = findActivePage();

        _splashScreenUI = okay::UI(okay::ui::image(*_splashScreenTex));
        _splashScreenUI.render(glm::vec2{}, SPLASH_UI_LAYER);

        // ABSOLUTE HACK WE REALLY NEED AN ASYNC/JOB SYSTEM
        okay::Renderer* r = okay::Engine.systems.getSystemChecked<okay::Renderer>();
        r->preTick();
        r->tick();
        r->postTick();

        if (hasCurrentPage()) {
            okay::Engine.logger.debug("Creating page enties");
            _pages[_currentPage].page->initializePage();
            std::this_thread::sleep_for(std::chrono::seconds(1));

            okay::TweenConfig<float> tweenConfig{
                .start = 1.0f,
                .end = 0.0f,
                .durationMs = 500,
                .easingFn = okay::easing::cubicIn,
                .onEnd =
                    [this]() {
                        _splashScreenUI.update(okay::ui::box());
                        _splashScreenUI.render(glm::vec2{}, SPLASH_UI_LAYER);
                        // _splashScreenUI.cleanup();
                    },
            };

            _splashAlphaTween = okay::Tween<float>::create(tweenConfig);
            _splashAlphaTween->start();
        }

        okay::Engine.logger.debug("Current page {}", _currentPage);
    }

    void preTick() {
        if (!_splashAlphaTween || _splashAlphaTween->isFinished()) {
            return;
        }

        _splashScreenUI.update(okay::ui::image(*_splashScreenTex)
                .backgroundColorSet(glm::vec4(1.0f, 1.0f, 1.0f, _splashAlphaTween->value())));

        _splashScreenUI.render(glm::vec2{}, SPLASH_UI_LAYER);
    }

    void tick() {
        std::size_t nextPage = findActivePage();

        if (nextPage == _currentPage) {
            return;
        }

        if (hasCurrentPage()) {
            okay::Engine.logger.debug("Freeing entites from page");
            _pages[_currentPage].page->closePage();
        }

        _currentPage = nextPage;

        okay::Engine.logger.debug("Current page {}", _currentPage);

        if (hasCurrentPage()) {
            okay::Engine.logger.debug("Creating page enties");
            _pages[_currentPage].page->initializePage();
        }
    }

    void addPage(PageEntry pe) {
        const bool isNumbered = pe.number.isSome();

        _pages.push_back(std::move(pe));

        if (isNumbered) {
            _numNumberedPages++;
        }
    }

    void switchPageLeft() {
        if (_numNumberedPages == 0) {
            return;
        }

        if (_currentNumberedPage == 0) {
            _currentNumberedPage = _numNumberedPages - 1;
        } else {
            _currentNumberedPage -= 1;
        }
    }

    void switchPageRight() {
        if (_numNumberedPages == 0) {
            return;
        }

        _currentNumberedPage = (_currentNumberedPage + 1) % _numNumberedPages;
    }

    void setNumberedPage(std::size_t page) {
        if (_numNumberedPages == 0) {
            _currentNumberedPage = 0;
            return;
        }

        _currentNumberedPage = page % _numNumberedPages;
    }

   private:
    void recountNumberedPages() {
        _numNumberedPages = 0;

        for (const PageEntry& page : _pages) {
            if (page.number.isSome()) {
                _numNumberedPages++;
            }
        }

        if (_numNumberedPages == 0) {
            _currentNumberedPage = 0;
        } else {
            _currentNumberedPage %= _numNumberedPages;
        }
    }

    bool hasCurrentPage() const {
        return _currentPage != NO_PAGE && _currentPage < _pages.size() &&
               _pages[_currentPage].page != nullptr;
    }

    bool pageActive(const PageEntry& pe) const {
        if (!pe.activeWhenPred()) {
            return false;
        }

        if (pe.number.isSome()) {
            return *pe.number == _currentNumberedPage;
        }

        return true;
    }

    bool pageForceOverrides(const PageEntry& pe) const {
        return pe.forceOverrideWhenPred();
    }

    std::size_t findBestActivePage() const {
        std::size_t bestPage = NO_PAGE;
        std::size_t bestPriority = 0;

        for (std::size_t i = 0; i < _pages.size(); ++i) {
            const PageEntry& entry = _pages[i];

            if (!pageActive(entry)) {
                continue;
            }

            if (bestPage == NO_PAGE || entry.priority > bestPriority) {
                bestPage = i;
                bestPriority = entry.priority;
            }
        }

        return bestPage;
    }

    std::size_t findActivePage() const {
        std::size_t bestPage = findBestActivePage();

        std::size_t currentPriority = 0;
        if (bestPage != NO_PAGE) {
            currentPriority = _pages[bestPage].priority;
        }

        for (std::size_t i = 0; i < _pages.size(); ++i) {
            const PageEntry& entry = _pages[i];

            if (!pageForceOverrides(entry)) {
                continue;
            }

            if (entry.priority > currentPriority) {
                bestPage = i;
                currentPriority = entry.priority;
            }
        }

        return bestPage;
    }

    static constexpr std::size_t NO_PAGE = static_cast<std::size_t>(-1);
    static constexpr std::uint8_t SPLASH_UI_LAYER = 5;

    std::vector<PageEntry> _pages;
    std::size_t _currentPage{NO_PAGE};
    std::size_t _currentNumberedPage{0};
    std::size_t _numNumberedPages{0};

    std::shared_ptr<okay::Tween<float>> _splashAlphaTween;
    okay::UI _splashScreenUI;
    okay::GameAssetRef<okay::Texture, okay::TextureLoadSettings> _splashScreenTex{
        "textures/splash.png"};
};

}  // namespace dash

#endif  // __PAGE_H__
