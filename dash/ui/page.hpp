#ifndef __PAGE_H__
#define __PAGE_H__

#include <okay/okay.hpp>

#include <functional>
#include <memory>
#include <utility>
#include <vector>

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
        return false;
    }};
    std::size_t priority{0};

    PageEntry() = default;

    explicit PageEntry(std::unique_ptr<IPage> newPage) : page(std::move(newPage)) {}

    static PageEntry create(std::unique_ptr<IPage> page) {
        return PageEntry(std::move(page));
    }

    PageEntry& activeWhen(std::function<bool()> fn) {
        activeWhenPred = std::move(fn);
        return *this;
    }

    PageEntry& withPriority(std::size_t newPriority) {
        priority = newPriority;
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
    }

    void initialize() {
        _currentPage = findActivePage();

        if (hasCurrentPage()) {
            okay::Engine.logger.debug("Creating page enties");
            _pages[_currentPage].page->initializePage();
        }

        okay::Engine.logger.debug("Current page {}", _currentPage);
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

   private:
    static constexpr std::size_t NO_PAGE = static_cast<std::size_t>(-1);

    std::vector<PageEntry> _pages;
    std::size_t _currentPage{NO_PAGE};

    bool hasCurrentPage() const {
        return _currentPage != NO_PAGE && _currentPage < _pages.size() &&
               _pages[_currentPage].page != nullptr;
    }

    std::size_t findActivePage() const {
        std::size_t bestPage = NO_PAGE;
        std::size_t bestPriority = 0;

        for (std::size_t i = 0; i < _pages.size(); ++i) {
            const PageEntry& entry = _pages[i];

            if (!entry.activeWhenPred()) {
                continue;
            }

            if (bestPage == NO_PAGE || entry.priority > bestPriority) {
                bestPage = i;
                bestPriority = entry.priority;
            }
        }

        return bestPage;
    }
};

}  // namespace dash

#endif  // __PAGE_H__
