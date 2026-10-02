import os
import json
from cryptography.fernet import Fernet

class AndrewRPC:
    def __init__(self, shared_key):
        self.fernet = Fernet(shared_key)
        self.pending_nonce = None

    def encrypt_msg(self, msg_dict):
        msg_bytes = json.dumps(msg_dict).encode('utf-8')
        return self.fernet.encrypt(msg_bytes)

    def decrypt_msg(self, ciphertext):
        msg_bytes = self.fernet.decrypt(ciphertext)
        return json.loads(msg_bytes.decode('utf-8'))

def main():
    shared_key = Fernet.generate_key()
    client = AndrewRPC(shared_key)
    server = AndrewRPC(shared_key)

    print("--- Andrew Secure RPC Handshake ---")
    
    # Message 1: A -> B (A, Na)
    Na = os.urandom(8).hex()
    msg1 = {"A": "Client", "Na": Na}
    print(f"Msg 1 (A->B): {msg1}")

    # Message 2: B -> A ({Na+1, Nb}Kab)
    Na_val = int(Na, 16)
    Nb_val = int(os.urandom(8).hex(), 16)
    server.pending_nonce = Nb_val
    msg2_pt = {"Na_plus_1": hex(Na_val + 1), "Nb": hex(Nb_val)}
    msg2_ct = server.encrypt_msg(msg2_pt)
    print(f"Msg 2 (B->A): Encrypted {msg2_pt}")

    # Message 3: A -> B ({Nb+1}Kab)
    dec_msg2 = client.decrypt_msg(msg2_ct)
    assert dec_msg2["Na_plus_1"] == hex(Na_val + 1), "Client: Nonce A mismatch!"
    
    Nb_from_server = int(dec_msg2["Nb"], 16)
    msg3_pt = {"Nb_plus_1": hex(Nb_from_server + 1)}
    msg3_ct = client.encrypt_msg(msg3_pt)
    print(f"Msg 3 (A->B): Encrypted {msg3_pt}")

    # Message 4: B -> A ({Ksession, Nb'}Kab)
    dec_msg3 = server.decrypt_msg(msg3_ct)
    if dec_msg3["Nb_plus_1"] != hex(server.pending_nonce + 1):
        raise ValueError("Server: Nonce B mismatch! Possible replay attack.")
    
    # Handshake successful, clear pending nonce
    server.pending_nonce = None

    session_key = Fernet.generate_key().decode('utf-8')
    Nb_prime = os.urandom(8).hex()
    msg4_pt = {"Ksession": session_key, "Nb_prime": Nb_prime}
    msg4_ct = server.encrypt_msg(msg4_pt)
    print("Msg 4 (B->A): Encrypted session key sent.")

    dec_msg4 = client.decrypt_msg(msg4_ct)
    print("Session established successfully.")

    print("\n--- Replay Attack Test ---")
    print("Attacker captures Msg 3 and replays it to Server...")
    
    # Server expects a specific nonce for an ongoing handshake. 
    # Let's say a new handshake started, so pending_nonce is different.
    server.pending_nonce = int(os.urandom(8).hex(), 16)
    
    try:
        replayed_dec = server.decrypt_msg(msg3_ct)
        if replayed_dec["Nb_plus_1"] != hex(server.pending_nonce + 1):
            print("Server rejects replayed message: Nonce mismatch. Replay attack blocked.")
        else:
            print("Server accepted replayed message (this should not happen).")
    except Exception as e:
        print(f"Replay attack failed: {e}")

if __name__ == "__main__":
    main()
