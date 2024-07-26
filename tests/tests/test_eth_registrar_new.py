
from web3 import Web3
from tests.utils import run_test, load_contract

contract_eth_registrar = load_contract(
    "253553366da8546fc250f225fe3d25d0c782303b"
)

# From : https://etherscan.io/tx/0x6792413131e66b71890dbf6484c1a73df539c91fe83a367fc53e0eca8825254b
def test_register_new(backend, firmware, navigator, test_name, wallet_addr):
    data = "0x74694a2b000000000000000000000000000000000000000000000000000000000000010000000000000000000000000068ceeb00a4af3be54fdce5c537f17f9b392d2ae70000000000000000000000000000000000000000000000000000000001e133809923eb9400000003f60f47f8815559d77b392dd7ac0b96ffb4b91d0616a0b996000000000000000000000000231b0ee14048e9dccd1d247744d114a4eb5e8e630000000000000000000000000000000000000000000000000000000000000140000000000000000000000000000000000000000000000000000000000000000100000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000005636b6336360000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000001000000000000000000000000000000000000000000000000000000000000002000000000000000000000000000000000000000000000000000000000000000a48b95dd7181616ea04d084f17ac9af18cda841da2cef9ae1fd28be519f17bcda509e05cae000000000000000000000000000000000000000000000000000000000000003c0000000000000000000000000000000000000000000000000000000000000060000000000000000000000000000000000000000000000000000000000000001468ceeb00a4af3be54fdce5c537f17f9b392d2ae700000000000000000000000000000000000000000000000000000000000000000000000000000000"
    run_test(
        contract_eth_registrar, 
        data, 
        backend, 
        firmware, 
        navigator, 
        test_name, 
        wallet_addr
    )