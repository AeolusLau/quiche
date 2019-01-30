// Copyright (c) 2013 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef QUICHE_QUIC_CORE_CRYPTO_P256_KEY_EXCHANGE_H_
#define QUICHE_QUIC_CORE_CRYPTO_P256_KEY_EXCHANGE_H_

#include <cstdint>

#include "base/macros.h"
#include "third_party/boringssl/src/include/openssl/base.h"
#include "net/third_party/quiche/src/quic/core/crypto/key_exchange.h"
#include "net/third_party/quiche/src/quic/platform/api/quic_export.h"
#include "net/third_party/quiche/src/quic/platform/api/quic_string.h"
#include "net/third_party/quiche/src/quic/platform/api/quic_string_piece.h"

namespace quic {

// P256KeyExchange implements a KeyExchange using elliptic-curve
// Diffie-Hellman on NIST P-256.
class QUIC_EXPORT_PRIVATE P256KeyExchange : public KeyExchange {
 public:
  ~P256KeyExchange() override;

  // New creates a new key exchange object from a private key. If
  // |private_key| is invalid, nullptr is returned.
  static std::unique_ptr<P256KeyExchange> New(QuicStringPiece private_key);

  // |NewPrivateKey| returns a private key, suitable for passing to |New|.
  // If |NewPrivateKey| can't generate a private key, it returns an empty
  // string.
  static QuicString NewPrivateKey();

  // KeyExchange interface.
  const Factory& GetFactory() const override;
  bool CalculateSharedKey(QuicStringPiece peer_public_value,
                          QuicString* shared_key) const override;
  void CalculateSharedKey(QuicStringPiece peer_public_value,
                          QuicString* shared_key,
                          std::unique_ptr<Callback> callback) const override;
  QuicStringPiece public_value() const override;

 private:
  enum {
    // A P-256 field element consists of 32 bytes.
    kP256FieldBytes = 32,
    // A P-256 point in uncompressed form consists of 0x04 (to denote
    // that the point is uncompressed) followed by two, 32-byte field
    // elements.
    kUncompressedP256PointBytes = 1 + 2 * kP256FieldBytes,
    // The first byte in an uncompressed P-256 point.
    kUncompressedECPointForm = 0x04,
  };

  // P256KeyExchange wraps |private_key|, and expects |public_key| consists of
  // |kUncompressedP256PointBytes| bytes.
  P256KeyExchange(bssl::UniquePtr<EC_KEY> private_key,
                  const uint8_t* public_key);
  P256KeyExchange(const P256KeyExchange&) = delete;
  P256KeyExchange& operator=(const P256KeyExchange&) = delete;

  bssl::UniquePtr<EC_KEY> private_key_;
  // The public key stored as an uncompressed P-256 point.
  uint8_t public_key_[kUncompressedP256PointBytes];
};

}  // namespace quic

#endif  // QUICHE_QUIC_CORE_CRYPTO_P256_KEY_EXCHANGE_H_