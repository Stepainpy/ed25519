# Edwards-curve Digital Signature Algorithm, Ed25519

> [!Warning]
> This code is not intended to provide true protection against real-world attacks. Instead, it serves as a demonstration of the algorithms.

Implementation of EdDSA on Ed25519.

## Documentation

All functions that return a value return `0` upon success/, nonzero value otherwise.

**`int ed25519_pvt_to_pub(ed25519_public_key_t pub_key, const ed25519_private_key_t pvt_key)`**  
Create public key from private key (seed).

**`int ed25519_keygen(ed25519_private_key_t pvt_key, ed25519_public_key_t pub_key)`**  
Generating private key (from cryptographic safe source) and immediately calculate public key.

**`int ed25519_sign(const void* data, size_t size, const ed25519_private_key_t pvt_key, ed25519_signature_t sign)`**  
Calculate signature for message and private key.

**`int ed25519_verify(const void* data, size_t size, const ed25519_public_key_t pub_key, const ed25519_signature_t sign)`**  
Verify message by signature and public key.
> [!IMPORTANT]
> Function returns NOT usual bool.  
> `0` -> Success  
> `1` -> Failure
