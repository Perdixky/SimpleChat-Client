#pragma once
#include <cstdint>
#include <format>
#include <rfl/Generic.hpp>
#include <boost/asio/ip/tcp.hpp>

template <>
struct std::formatter<boost::asio::ip::tcp::endpoint> : std::formatter<std::string> {
    auto format(const boost::asio::ip::tcp::endpoint& endpoint, std::format_context& ctx) const {
        std::string s = endpoint.address().to_string() + ":" + std::to_string(endpoint.port());
        return std::formatter<std::string>::format(s, ctx);
    }
};

// Formatter for rfl::Object<rfl::Generic>
template <>
struct std::formatter<rfl::Object<rfl::Generic>, char> {
    template <typename ParseContext>
    constexpr auto parse(ParseContext& ctx) {
        return ctx.begin();
    }

    template <typename FormatContext>
    auto format(const rfl::Object<rfl::Generic>& obj, FormatContext& ctx) const {
        auto out = ctx.out();
        out = std::format_to(out, "{{");
        bool first = true;
        for (const auto& [key, value] : obj) {
            if (!first) {
                out = std::format_to(out, ", ");
            }
            out = std::format_to(out, "\"{{}}\": {{}}", key, value);
            first = false;
        }
        return std::format_to(out, "}}");
    }
};

// Formatter for std::vector<rfl::Generic> (rfl::Array)
template <>
struct std::formatter<std::vector<rfl::Generic>, char> {
    template <typename ParseContext>
    constexpr auto parse(ParseContext& ctx) {
        return ctx.begin();
    }

    template <typename FormatContext>
    auto format(const std::vector<rfl::Generic>& arr, FormatContext& ctx) const {
        auto out = ctx.out();
        out = std::format_to(out, "[");
        bool first = true;
        for (const auto& item : arr) {
            if (!first) {
                out = std::format_to(out, ", ");
            }
            out = std::format_to(out, "{{}}", item);
            first = false;
        }
        return std::format_to(out, "]");
    }
};

// Formatter for rfl::Generic
template <>
struct std::formatter<rfl::Generic, char> {
    template <typename ParseContext>
    constexpr auto parse(ParseContext& ctx) {
        return ctx.begin();
    }

    template <typename FormatContext>
    auto format(const rfl::Generic& gen, FormatContext& ctx) const {
        return std::visit(
            [&](const auto& value) {
                return std::format_to(ctx.out(), "{{}}", value);
            },
            static_cast<const std::variant<bool, int64_t, double, std::string, rfl::Object<rfl::Generic>, std::vector<rfl::Generic>, std::nullopt_t>&>(gen.get())
        );
    }
};

// Overload for std::nullopt_t formatter
template <>
struct std::formatter<std::nullopt_t, char> {
    template <typename ParseContext>
    constexpr auto parse(ParseContext& ctx) {
        return ctx.begin();
    }

    template <typename FormatContext>
    auto format(std::nullopt_t, FormatContext& ctx) const {
        return std::format_to(ctx.out(), "null");
    }
};
