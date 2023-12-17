use std::ffi::CString;

use ffi::UtreexoHash;
pub mod ffi;

pub struct Forest {
    forest: *const ffi::utreexo_forest,
}

///# Safety
/// It's ok to send the forest to another thread because it's just a pointer
/// to a C struct. The C struct is heap allocated and the pointer is valid
/// for the lifetime of the struct.
unsafe impl Send for Forest {}

pub struct Configs<'a> {
    map_filename: &'a str,
    forest_filename: &'a str,
}

impl Drop for Forest {
    fn drop(&mut self) {
        unsafe {
            ffi::utreexo_forest_free(self.forest);
        }
    }
}

impl Forest {
    pub fn new(conf: Configs) -> Forest {
        let mut forest: *const ffi::utreexo_forest = core::ptr::null();
        let name = CString::new(conf.map_filename).unwrap();
        let name2 = CString::new(conf.forest_filename).unwrap();
        let res = unsafe {
            ffi::utreexo_forest_init(
                &mut forest,
                name.as_c_str().as_ptr(),
                name2.as_c_str().as_ptr(),
            )
        };
        if res != 0 {
            panic!("Could not create a forest");
        }
        Forest { forest }
    }

    pub fn modify(&self, utxos: &[UtreexoHash], stxos: &[UtreexoHash]) {
        let putxos = utxos.as_ptr();
        let pstxos = stxos.as_ptr();

        unsafe {
            ffi::utreexo_forest_modify(
                self.forest,
                putxos,
                utxos.len() as core::ffi::c_int,
                pstxos,
                stxos.len() as core::ffi::c_int,
            );
        }
    }
}

fn main() {
    let forest = Forest::new(crate::Configs {
        map_filename: "test.bin",
        forest_filename: "forest_test.bin",
    });
    let leaves = (0..8)
        .into_iter()
        .map(|n| UtreexoHash([n; 32]))
        .collect::<Vec<_>>();
    forest.modify(&leaves, &[]);
    //forest.modify(&[], &[leaves[0]]);
}
#[cfg(test)]
mod tests {
    use crate::{ffi::UtreexoHash, Forest};

    #[test]
    fn test() {
        let forest = Forest::new(crate::Configs {
            map_filename: "test.bin",
            forest_filename: "forest_test.bin",
        });
        let leaves = (0..8)
            .into_iter()
            .map(|n| UtreexoHash([n; 32]))
            .collect::<Vec<_>>();
        forest.modify(&leaves, &[]);
        //forest.modify(&[], &[leaves[0]]);
    }
}
