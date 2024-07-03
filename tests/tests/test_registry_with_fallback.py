
from web3 import Web3
from tests.utils import run_test, load_contract

contract_registry_with_fallback = load_contract(
    "00000000000c2e074ec69a0dfb2997ba6c7d2e1e"
)

# From : https://etherscan.io/tx/0x7a6335c07bfc09c960fb3ed774c019a511c2011ade1408c48fa8502b4403230c
def test_set_owner(backend, firmware, navigator, test_name, wallet_addr):
    data = "0x5b0fc9c3f70f0976117fe1a364173f15e56c723e15e4d1def7e882d68ff5126b53305f890000000000000000000000001a7006e11fe389629fbc24c75a90ca2979e798dd"
    run_test(
        contract_registry_with_fallback, 
        data, 
        backend, 
        firmware, 
        navigator, 
        test_name, 
        wallet_addr
    )

# From : https://etherscan.io/tx/0xdf3fbe0737bdcbc3d8a88a5f68b4b9243e56ba0bad527b6911ce58867f378c3d
def test_set_subnode_resolver(backend, firmware, navigator, test_name, wallet_addr):
    data = "0x5ef2c7f0b80478f266d748625c0d3208f7ded727374ab3f3a4f30c230e6dbbb62f462b5afd341581598f70c2162381faf4328118039b3b0e71e7cd41ea4b3eb2411e9d330000000000000000000000004e903fb3c9e37ab1ed595172781a11859b52c4c80000000000000000000000004976fb03c32e5b8cfe2b6ccb31c09ba78ebaba410000000000000000000000000000000000000000000000000000000000000000"
    run_test(
        contract_registry_with_fallback, 
        data, 
        backend, 
        firmware, 
        navigator, 
        test_name, 
        wallet_addr
    )
