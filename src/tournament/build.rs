use cxx_qt_build::{CxxQtBuilder, QmlModule};

fn main() {
    CxxQtBuilder::new()
        .qml_module(QmlModule::<_, &str> {
            uri: "org.kde.chessament.tournament",
            rust_files: &["src/player.rs"],
            ..Default::default()
        })
        .build();
}
