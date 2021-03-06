/*
    This file is part of Helio Workstation.

    Helio is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    Helio is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with Helio. If not, see <http://www.gnu.org/licenses/>.
*/

#pragma once

#include "Serializable.h"
#include "RequestTranslationsThread.h"

class TranslationManager :
    public ChangeBroadcaster,
    private Serializable,
    private Timer,
    private RequestTranslationsThread::Listener
{
public:

    static TranslationManager &getInstance()
    {
        static TranslationManager Instance;
        return Instance;
    }

    typedef struct
    {
        String localeId;
        String localeName;
        String localeAuthor;
    } Locale;
    
    static File getDownloadedTranslationsFile();
    static File getDebugTranslationsFile();
    
    void initialise(const String &commandLine);
    void shutdown();

    StringPairArray getMappings() const;
    String findPluralFor(const String &baseLiteral, int64 targetNumber);

    Array<Locale> getAvailableLocales() const;
    void loadLocaleWithName(const String &localeName);
    void loadLocaleWithId(const String &localeId);

    String getCurrentLocaleName() const; // i.e. "Русский"
    String getCurrentLocaleId() const; // i.e. "ru"
    
    void reloadLocales();

    
    //===------------------------------------------------------------------===//
    // Static
    //===------------------------------------------------------------------===//
    
    static String translate(const String &text);
    static String translate(const String &text, const String &resultIfNotFound);
    static String translate(const String &baseLiteral, int64 targetNumber);
    
private:
    
    //===------------------------------------------------------------------===//
    // Serializable
    //===------------------------------------------------------------------===//
    
    XmlElement *serialize() const override;
    void deserialize(const XmlElement &xml) override;
    void reset() override;
    
private:
    
    ScopedPointer<RequestTranslationsThread> requestTranslationsThread;

    void translationsRequestOk() override;
    void translationsRequestFailed() override;
    
private:
    
    TranslationManager() {}
    
    void timerCallback() override;
    
    ScopedPointer<JavascriptEngine> engine;
    String pluralEquation;
    String equationResult;

    StringPairArray singulars;
    HashMap<String, StringPairArray> plurals;

    HashMap<String, Locale> availableTranslations;
    String getLocalizationFileContents() const;
    void loadFromXml(const String &xmlData);
    
    String getSelectedLocaleId() const;
    void setSelectedLocaleId(const String &localeId);
    
    friend struct PluralEquationWrapper;
    
private:

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(TranslationManager)

};
