#[cxx_qt::bridge]
pub mod qobject {

    unsafe extern "C++" {
        include!("cxx-qt-lib/qstring.h");
        type QString = cxx_qt_lib::QString;
    }

    extern "RustQt" {
        #[qobject]
        #[qml_element]
        type Event = super::EventRust;

        fn open(self: &Event);

        #[cxx_name = "createTournament"]
        fn create_tournament(self: &Event) -> Tour;

        #[cxx_name = "importTournament"]
        fn import_tournament(self: &Event, filename: QString);
    }
}

pub use crate::tournament::qobject::Tour;
use cxx_qt_lib::QString;

#[derive(Default)]
pub struct EventRust {
    tournaments: Vec<*mut Tour>,
}

impl qobject::Event {
    pub fn number_of_tournaments(&self) -> i32 {
        todo!();
    }

    pub fn open(&self) {
        todo!();
    }

    pub fn get_tournament(&self, i: i32) -> *const Tour {
        todo!();
    }

    pub fn create_tournament(&self) -> Result<*mut Tour> {
        todo!();
    }

    pub fn import_tournament(&self, filename: QString) {
        todo!();
    }

    pub fn save_as(&self, filename: QString) {
        todo!();
    }
}
