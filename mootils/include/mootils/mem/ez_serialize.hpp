#pragma once
#include <cstdint>
#include <cstddef>
#include <span>
#include "mootils/mem/serializer.hpp"
#include "mootils/print/print_safe.hpp"

namespace mem {
    template <typename T>
    concept Serializable = requires(T a, mem::ByteWriter& writer, mem::SizeCounter& counter, mem::ByteReader& reader) {
        { a.serialize(writer) } -> std::same_as<std::size_t>;
        { a.serialized_size_of(counter) } -> std::same_as<std::size_t>;
        { a.deserialize(reader) } -> std::same_as<std::size_t>;
    };

    template <Serializable T>
    std::size_t serialized_size_of(const T& obj) noexcept {
        mem::SizeCounter counter{};
        return obj.serialized_size_of(counter);
    }

    template <Serializable H>
    std::size_t serialize_hdr(mem::ByteWriter& writer, const H& hdr) noexcept {
        hdr.serialize(writer);
        return writer.used();
    }

    template <Serializable H>
    std::size_t serialize_hdr(std::span<std::byte> span, const H& hdr) noexcept {
        mem::ByteWriter writer{span};
        hdr.serialize(writer);
        return writer.used();
    }

    template <Serializable P>
    std::size_t serialize_payload(mem::ByteWriter& writer, const P& payload) noexcept {
        payload.serialize(writer);
        return writer.used();
    }

    template <Serializable P>
    std::size_t serialize_payload(std::span<std::byte> span, const P& payload) noexcept {
        mem::ByteWriter writer{span};
        payload.serialize(writer);
        return writer.used();
    }

    template <Serializable H, Serializable P>
    std::size_t serialize(mem::ByteWriter& writer, const H& hdr, const P& payload) noexcept {
        const std::size_t before = writer.used();
        serialize_hdr(writer, hdr);
        serialize_payload(writer, payload);
        return writer.used() - before;
    }

    template <Serializable H, Serializable P>
    std::size_t serialize(std::span<std::byte> span, const H& hdr, const P& payload) noexcept {
        mem::ByteWriter writer{span};
        serialize_hdr(writer, hdr);
        serialize_payload(writer, payload);
        return writer.used();
    }

    template <Serializable H>
    std::size_t deserialize_hdr(mem::ByteReader& reader, H& out) noexcept {
        const std::size_t before = reader.used();
        out.deserialize(reader);
        return reader.used() - before;
    }

    template <Serializable H>
    std::size_t deserialize_hdr(std::span<std::byte> span, H& out) noexcept {
        mem::ByteReader reader{span};
        out.deserialize(reader);
        return reader.used();
    }

    template <Serializable P>
    std::size_t deserialize_payload(mem::ByteReader& reader, P& out) noexcept {
        // payloads can have strings which means they have variable size depending on the
        // length of the string, so we cannot know the actual size of P from the type alone
        // check the minimum size of P as a best guess

        const std::size_t before = reader.used();
        out.deserialize(reader);
        return reader.used() - before;
    }

    template <Serializable P>
    std::size_t deserialize_payload(std::span<std::byte> span, P& out) noexcept {
        // payloads can have strings which means they have variable size depending on the
        // length of the string, so we cannot know the actual size of P from the type alone
        // check the minimum size of P as a best guess

        mem::ByteReader reader{span};
        out.deserialize(reader);
        return reader.used();
    }

    template <Serializable H, Serializable P>
    std::size_t deserialize(mem::ByteReader& reader, H& hdr, P& payload) noexcept {
        // payloads can have strings which means they have variable size depending on the
        // length of the string, so we cannot know the actual size of P from the type alone
        // check the minimum size of P as a best guess

        const std::size_t before = reader.used();
        deserialize_hdr(reader, hdr);
        deserialize_payload(reader, payload);
        return reader.used() - before;
    }

    template <Serializable H, Serializable P>
    std::size_t deserialize(std::span<std::byte> span, H& hdr, P& payload) noexcept {
        // payloads can have strings which means they have variable size depending on the
        // length of the string, so we cannot know the actual size of P from the type alone
        // check the minimum size of P as a best guess

        mem::ByteReader reader{span};
        deserialize_hdr(reader, hdr);
        deserialize_payload(reader, payload);
        return reader.used();
    }
}