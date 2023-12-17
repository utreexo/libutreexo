use core::ffi::{c_int, c_char};

#[repr(C)]
#[derive(Clone)]
pub struct UtreexoHash(pub [u8; 32]);

impl Copy for UtreexoHash{}

#[repr(C)]
#[allow(non_camel_case_types)]
pub struct utreexo_forest;

#[allow(improper_ctypes)]
#[link(name = "utreexo", kind = "static")]
#[link(name = "crypto")]
extern "C" {
    pub fn utreexo_forest_init(
        p: *mut *const utreexo_forest,
        map_name: *const c_char,
        forest_name: *const c_char,
    ) -> c_int;
    pub fn utreexo_forest_free(p: *const utreexo_forest) -> c_int;
    pub fn utreexo_forest_modify(
        p: *const utreexo_forest,
        utxos: *const UtreexoHash,
        utxo_count: c_int,
        stxos: *const UtreexoHash,
        stxo_count: c_int,
    ) -> c_int;
}
