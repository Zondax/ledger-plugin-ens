
from web3 import Web3
from tests.utils import run_test, load_contract

contract_eth_registrar = load_contract(
    "283af0b28c62c092c9727f1ee09c02ca627eb7f5"
)

def test_commit(backend, firmware, navigator, test_name, wallet_addr):
    data = "0xf14fcbc86952f78134ad22871b951c2f5d1bab4f5f33359c35866dbbf2464993ee2b5589"
    run_test(
        contract_eth_registrar, 
        data, 
        backend, 
        firmware, 
        navigator, 
        test_name, 
        wallet_addr
    )
