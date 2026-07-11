#pragma once

#include <memory>
#include <cstdint>
#include <cstddef>
#include <span>
#include <vector>
#include "mootils/sock/tcp_socket.hpp"
#include "mootils/mem/arena.hpp"
#include "mootils/mem/serializer.hpp"
#include "mootils/mem/ez_serialize.hpp"

namespace sock {
    // recv exactly size bytes, return false if failure (likely disconnected), true on success
    bool recv_exact(std::shared_ptr<sock::TCPClient> sock, void* buf, std::size_t size);

    bool recv_exact(std::shared_ptr<sock::TCPClient> sock, std::span<std::byte> buf);

    // drain size bytes from tcp client recv buffer
    bool drain(std::shared_ptr<sock::TCPClient> sock, std::size_t size);

    template <typename Hdr, typename Payload>
    void send(std::shared_ptr<sock::TCPClient> sock,
            mem::Arena& arena,
            Hdr& hdr,
            const Payload& payload) {

        std::size_t hdr_size = mem::serialized_size_of(hdr);
        std::size_t payload_size = mem::serialized_size_of(payload);
        hdr.payload_size = static_cast<std::uint32_t>(payload_size);


        auto allocate_result = arena.allocate(hdr_size + payload_size);
        if (!allocate_result) {
            throw std::runtime_error("Failed allocate memory for header and payload");
        }

        std::span<std::byte> buf = allocate_result.value();
        mem::ByteWriter writer{buf};
        mem::serialize(writer, hdr, payload);

        auto send_result = sock->send_all(buf);
        if (!send_result.ok()) {
            throw std::runtime_error("Failed to send message");
        }
    }

    template <typename Hdr, typename Payload>
    void send(std::shared_ptr<sock::TCPClient> sock,
            mem::Arena& arena,
            Hdr& hdr,
            const std::vector<Payload>& payload) {

        std::size_t hdr_size = mem::serialized_size_of(hdr);
        std::size_t payload_size = 0;
        for (const auto& p : payload) {
            payload_size += mem::serialized_size_of(p);
        }
        hdr.payload_size = static_cast<std::uint32_t>(payload_size);

        auto allocate_result = arena.allocate(hdr_size + payload_size);
        if (!allocate_result) {
            throw std::runtime_error("Failed allocate memory for header and payload");
        }

        std::span<std::byte> buf = allocate_result.value();
        mem::ByteWriter writer{buf};
        mem::serialize_hdr(writer, hdr);

        for (const auto& p : payload) {
            mem::serialize_payload(writer, p);
        }

        auto send_result = sock->send_all(buf);
        if (!send_result.ok()) {
            throw std::runtime_error("Failed to send message");
        }
    }

    template <typename Hdr>
    void send(std::shared_ptr<sock::TCPClient> sock,
            mem::Arena& arena,
            Hdr& hdr) {

        std::size_t hdr_size = mem::serialized_size_of(hdr);
        hdr.payload_size = 0;

        auto allocate_result = arena.allocate(hdr_size);
        if (!allocate_result) {
            throw std::runtime_error("Failed allocate memory for header");
        }

        std::span<std::byte> buf = allocate_result.value();
        mem::serialize_hdr(buf, hdr);

        auto send_result = sock->send_all(buf);
        if (!send_result.ok()) {
            throw std::runtime_error("Failed to send message");
        }
    }

    template <typename Hdr>
    bool recv_hdr(std::shared_ptr<sock::TCPClient> sock,
            mem::Arena& arena,
            Hdr& hdr) {
        std::size_t hdr_size = mem::serialized_size_of(hdr);

        auto allocate_result = arena.allocate(hdr_size);
        if (!allocate_result) {
            throw std::runtime_error("Header size exceeds arena size");
        }

        std::span<std::byte> span = allocate_result.value();
        auto recv_result = recv_exact(sock, span);
        if (!recv_result) {
            // disconnected or error and closed
            return false;
        }

        mem::ByteReader reader{span};
        mem::deserialize_hdr(reader, hdr);
        return true;
    }

    template <typename Payload>
    bool recv_payload(std::shared_ptr<sock::TCPClient> sock,
            mem::Arena& arena,
            Payload& payload,
            std::size_t payload_size) {

        auto allocate_result = arena.allocate(payload_size);
        if (!allocate_result) {
            throw std::runtime_error("Payload size exceeds arena size");
        }

        std::span<std::byte> span = allocate_result.value();
        auto recv_result = recv_exact(sock, span);
        if (!recv_result) {
            // disconnected or error and closed
            return false;
        }

        mem::ByteReader reader{span};
        mem::deserialize_payload(reader, payload);
        return true;
    }
}
