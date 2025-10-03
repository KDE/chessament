#[cxx_qt::bridge]
pub mod qobject {
    unsafe extern "C++" {
        include!("cxx-qt-lib/qstring.h");
        type QString = cxx_qt_lib::QString;

        include!("cxx-qt-lib/qurl.h");
        type QUrl = cxx_qt_lib::QUrl;

        include!("cxx-qt-lib/qdatetime.h");
        type QDateTime = cxx_qt_lib::QDateTime;
    }

    #[auto_cxx_name]
    extern "RustQt" {
        #[qobject]
        #[qproperty(QUrl, url)]
        #[qproperty(QDateTime, last_updated)]
        type RatingList = super::RatingListRust;
    }

    //impl cxx_qt::Initialize for RatingList {}
}

use cxx_qt_lib::{QDateTime, QString, QUrl};

#[derive(Default)]
pub struct RatingListRust {
    url: QUrl,
    last_updated: QDateTime,
}

impl qobject::RatingList {
    pub fn import_list(url: QUrl) -> Result<(), QString> {
        let client = reqwest::blocking::Client::builder()
            .user_agent("Chessament (https://invent.kde.org/games/chessament/)")
            .build();

        Ok(())
    }
}
