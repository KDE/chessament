#[cxx_qt::bridge]
pub mod qobject {
    unsafe extern "C++" {
        include!("cxx-qt-lib/qstring.h");
        type QString = cxx_qt_lib::QString;
    }

    extern "RustQt" {
        #[qobject]
        #[qml_element]
        #[qproperty(QString, name)]
        #[qproperty(QString, city)]
        #[qproperty(QString, federation)]
        pub type Tour = super::TournamentRust;
    }
}

use std::pin::Pin;

use cxx_qt::CxxQtType;
use qobject::QString;

use crate::player::qobject::Player;

#[derive(Default)]
pub struct TournamentRust {
    name: QString,
    city: QString,
    federation: QString,
    players: Vec<*mut Player>,
}

impl qobject::Tour {
    pub fn add_player(mut self: Pin<&mut Self>, player: *mut Player) {
        self.as_mut().rust_mut().players.push(player);
    }

    pub fn number_of_players(&self) -> i32 {
        self.players.len() as i32
    }

    pub fn number_of_rated_players(&self) -> i32 {
        self.players.iter().filter(|_player| true).count() as i32
    }
}

#[cfg(test)]
mod tests {
    #[test]
    fn sum() {
        assert_eq!(2 + 2, 4);
    }
}
