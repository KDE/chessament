#[cxx_qt::bridge]
pub mod qobject {
    unsafe extern "C++" {
        include!("cxx-qt-lib/qstring.h");
        type QString = cxx_qt_lib::QString;
    }

    #[qenum]
    #[namespace = "Players"]
    enum Title {
        // None,
        GM,
        IM,
        FM,
        CM,
        WGM,
        GIM,
        GFM,
        WCM,
    }

    #[auto_cxx_name]
    extern "RustQt" {
        #[qobject]
        #[qml_element]
        #[qproperty(i32, id)]
        #[qproperty(i32, starting_rank)]
        #[qproperty(Title, title)]
        #[qproperty(QString, name)]
        #[qproperty(i32, rating)]
        #[qproperty(i32, national_rating)]
        #[qproperty(QString, player_id)]
        #[qproperty(QString, birth_date)]
        #[qproperty(QString, federation)]
        #[qproperty(QString, origin)]
        #[qproperty(QString, sex)]
        type Player = super::PlayerRust;

        // fn title(self: &Player) -> Title;
    }

    impl cxx_qt::Initialize for Player {}
}

use cxx_qt_lib::QString;
use qobject::Title;

#[derive(Default)]
pub struct PlayerRust {
    id: i32,
    starting_rank: i32,
    title: Title,
    name: QString,
    rating: i32,
    national_rating: i32,
    player_id: QString,
    birth_date: QString,
    federation: QString,
    origin: QString,
    sex: QString,
}

impl qobject::Player {
    /*pub fn title(&self) -> qobject::Title {
        &self.title
    }*/
    /*pub fn title_for_string(&self, title: &QString) -> Title {
        match title.to_lower() {
            "gm" => Title::GM,
            "im" => Title::GM,
            "fm" => Title::GM,
            "cm" => Title::GM,
            "wgm" => Title::GM,
            "wim" => Title::GM,
            "wfm" => Title::GM,
            "wcm" => Title::GM,
            _ => Title::None,
        }
    }*/
}
