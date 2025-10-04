#include "../include/simpletranslator.h"

#include <QStringLiteral>

SimpleTranslator::SimpleTranslator(QObject* parent)
    : QTranslator(parent) {
}

bool SimpleTranslator::loadLanguage(const QString& locale) {
    m_translations.clear();

    const QString normalized = locale.toLower();
    if (!normalized.startsWith(QStringLiteral("fr"))) {
        return false;
    }

    auto add = [this](const char* ctx, const char* source, const char* translation) {
        m_translations.insert(Key(QString::fromUtf8(ctx), QString::fromUtf8(source)), QString::fromUtf8(translation));
    };

    add("CreditsDialog", "Credits", "Crédits");
    add("CreditsDialog", "SCREENME", "SCREENME");
    add("CreditsDialog", "Creative Credits", "Crédits créatifs");
    add("CreditsDialog", "Crafted with ❤️ by Sorok Dva. Empowering faster, privacy-first captures across all your displays.",
        "Conçu avec ❤️ par Sorok Dva. Capturez plus vite, en toute confidentialité, sur tous vos écrans.");
    add("CreditsDialog", "Design direction & product vision: <b>Sorok Dva</b>",
        "Direction artistique &amp; vision produit : <b>Sorok Dva</b>");
    add("CreditsDialog", "Tech stack: Qt Widgets · Custom Tailwind-inspired styling",
        "Pile technique : Qt Widgets · Style Tailwind personnalisé");
    add("CreditsDialog", "Special thanks to contributors and early adopters for feedback.",
        "Merci aux contributeurs et early adopters pour leurs retours.");
    add("CreditsDialog", "Voir mon GitHub", "Voir mon GitHub");
    add("CreditsDialog", "Voir mon site", "Voir mon site");
    add("CreditsDialog", "Back to app", "Retour à l'application");

    add("Editor", "Freehand", "Stylet");
    add("Editor", "Text", "Texte");
    add("Editor", "Rectangle", "Rectangle");
    add("Editor", "Ellipse", "Ellipse");
    add("Editor", "Line", "Ligne");
    add("Editor", "Arrow", "Flèche");
    add("Editor", "Save", "Enregistrer");
    add("Editor", "Copy", "Copier");
    add("Editor", "Upload", "Publier");
    add("Editor", "Print", "Imprimer");
    add("Editor", "Search", "Rechercher");
    add("Editor", "Close", "Fermer");

    add("MainWindow", "Unable to capture desktop", "Impossible de capturer le bureau");

    add("OptionsWindow", "ScreenMe Options", "Options ScreenMe");
    add("OptionsWindow", "Screenshot Hotkey:", "Raccourci capture :");
    add("OptionsWindow", "Press any key...", "Appuyez sur une touche...");
    add("OptionsWindow", "Fullscreen Screenshot Hotkey:", "Raccourci capture plein écran :");
    add("OptionsWindow", "File Extension:", "Extension de fichier :");
    add("OptionsWindow", "Image Quality:", "Qualité d'image :");
    add("OptionsWindow", "Default Save Folder:", "Dossier d'enregistrement :");
    add("OptionsWindow", "Browse", "Parcourir");
    add("OptionsWindow", "Language:", "Langue :");
    add("OptionsWindow", "English", "Anglais");
    add("OptionsWindow", "Français", "Français");
    add("OptionsWindow", "Start with system", "Démarrer avec le système");
    add("OptionsWindow", "Save", "Enregistrer");
    add("OptionsWindow", "Select Folder", "Sélectionner un dossier");
    add("OptionsWindow", "Language updated", "Langue mise à jour");
    add("OptionsWindow", "Restart ScreenMe to apply the new language.",
        "Redémarrez ScreenMe pour appliquer la nouvelle langue.");

    add("QObject", "Login to ScreenMe", "Se connecter à ScreenMe");
    add("QObject", "Take Screenshot", "Capturer une zone");
    add("QObject", "Take Fullscreen Screenshot", "Capturer tout l'écran");
    add("QObject", "Credits", "Crédits");
    add("QObject", "❓Help", "❓Aide");
    add("QObject", "🛠️ Report a bug", "🛠️ Signaler un bug");
    add("QObject", "Options", "Options");
    add("QObject", "Check for update", "Rechercher une mise à jour");
    add("QObject", "Exit", "Quitter");
    add("QObject", "My Gallery", "Ma galerie");
    add("QObject", "Logout", "Se déconnecter");
    add("QObject", "My Gallery (%1)", "Ma galerie (%1)");
    add("QObject", "Press the configured key combination to take a screenshot",
        "Utilisez le raccourci configuré pour réaliser une capture");
    add("QObject", "Login Successful", "Connexion réussie");
    add("QObject", "Connected as %1", "Connecté en tant que %1");
    add("QObject", "ScreenMe is already running", "ScreenMe est déjà en cours d'exécution");
    add("QObject", "An instance of this application is already running. Please quit the existing ScreenMe process first.",
        "Une instance de ScreenMe est déjà ouverte. Veuillez fermer l'application existante avant de relancer.");
    add("QObject", "Screenshot saved", "Capture enregistrée");
    add("QObject", "Fullscreen capture stored at %1", "Capture plein écran enregistrée dans %1");

    add("ScreenshotDisplay", "ScreenMe Capture", "Capture ScreenMe");
    add("ScreenshotDisplay", "Print Screenshot", "Imprimer la capture");

    return true;
}

QString SimpleTranslator::translate(const char* context, const char* sourceText, const char*, int) const {
    auto it = m_translations.constFind(Key(QString::fromUtf8(context), QString::fromUtf8(sourceText)));
    if (it != m_translations.constEnd()) {
        return it.value();
    }
    return QString();
}
