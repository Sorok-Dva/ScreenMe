#pragma once

#include <QHash>
#include <QTranslator>
#include <QString>
#include <QPair>

class SimpleTranslator : public QTranslator {
public:
    explicit SimpleTranslator(QObject* parent = nullptr);
    bool loadLanguage(const QString& locale);

    QString translate(const char* context, const char* sourceText, const char* disambiguation, int n) const override;

private:
    using Key = QPair<QString, QString>;
    QHash<Key, QString> m_translations;
};
