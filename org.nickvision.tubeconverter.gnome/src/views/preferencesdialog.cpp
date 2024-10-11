#include "views/preferencesdialog.h"
#include <libnick/localization/gettext.h>
#include <libnick/system/environment.h>

using namespace Nickvision::Events;
using namespace Nickvision::System;
using namespace Nickvision::TubeConverter::GNOME::Helpers;
using namespace Nickvision::TubeConverter::Shared::Controllers;
using namespace Nickvision::TubeConverter::Shared::Models;

namespace Nickvision::TubeConverter::GNOME::Views
{
    PreferencesDialog::PreferencesDialog(const std::shared_ptr<PreferencesViewController>& controller, GtkWindow* parent)
        : DialogBase{ parent, "preferences_dialog" },
        m_controller{ controller }
    {
        //Load
        DownloaderOptions options{ m_controller->getDownloaderOptions() };
        adw_combo_row_set_selected(m_builder.get<AdwComboRow>("themeRow"), static_cast<unsigned int>(m_controller->getTheme()));
        adw_combo_row_set_selected(m_builder.get<AdwComboRow>("completedNotificationTriggerRow"), static_cast<unsigned int>(m_controller->getCompletedNotificationPreference()));
        adw_switch_row_set_active(m_builder.get<AdwSwitchRow>("preventSuspendRow"), m_controller->getPreventSuspend());
        adw_combo_row_set_selected(m_builder.get<AdwComboRow>("historyLengthRow"), static_cast<unsigned int>(m_controller->getHistoryLengthIndex()));
        adw_switch_row_set_active(m_builder.get<AdwSwitchRow>("overwriteExistingFilesRow"), options.getOverwriteExistingFiles());
        adw_spin_row_set_value(m_builder.get<AdwSpinRow>("maxNumberOfActiveDownloadsRow"), static_cast<double>(options.getMaxNumberOfActiveDownloads()));
        adw_switch_row_set_active(m_builder.get<AdwSwitchRow>("limitCharactersRow"), options.getLimitCharacters());
        adw_switch_row_set_active(m_builder.get<AdwSwitchRow>("downloadImmediatelyRow"), m_controller->getDownloadImmediatelyAfterValidation());
        adw_switch_row_set_active(m_builder.get<AdwSwitchRow>("includeAutoGeneratedSubtitlesRow"), options.getIncludeAutoGeneratedSubtitles());
        adw_combo_row_set_selected(m_builder.get<AdwComboRow>("preferredVideoCodecRow"), static_cast<unsigned int>(options.getPreferredVideoCodec()));
        adw_spin_row_set_value(m_builder.get<AdwSpinRow>("speedLimitRow"), static_cast<double>(options.getSpeedLimit()));
        adw_switch_row_set_active(m_builder.get<AdwSwitchRow>("sponsorBlockRow"), options.getYouTubeSponsorBlock());
        gtk_editable_set_text(m_builder.get<GtkEditable>("proxyUrlRow"), options.getProxyUrl().c_str());
        adw_combo_row_set_selected(m_builder.get<AdwComboRow>("cookiesBrowserRow"), static_cast<unsigned int>(options.getCookiesBrowser()));
        adw_action_row_set_subtitle(m_builder.get<AdwActionRow>("cookiesFileRow"), options.getCookiesPath().filename().string().c_str());
        gtk_widget_set_tooltip_text(m_builder.get<GtkWidget>("cookiesFileRow"), options.getCookiesPath().string().c_str());
        adw_switch_row_set_active(m_builder.get<AdwSwitchRow>("embedMetadataRow"), options.getEmbedMetadata());
        adw_switch_row_set_active(m_builder.get<AdwSwitchRow>("embedChaptersRow"), options.getEmbedChapters());
        adw_switch_row_set_active(m_builder.get<AdwSwitchRow>("embedSubtitlesRow"), options.getEmbedSubtitles());
        adw_switch_row_set_active(m_builder.get<AdwSwitchRow>("cropAudioThumbnailRow"), options.getCropAudioThumbnails());
        adw_switch_row_set_active(m_builder.get<AdwSwitchRow>("removeSourceDataRow"), options.getRemoveSourceData());
        std::vector<std::string> postprocessingThreads;
        for(int i = 1; i <= m_controller->getMaxPostprocessingThreads(); i++)
        {
            postprocessingThreads.push_back(std::to_string(i));
        }
        GtkHelpers::setComboRowModel(m_builder.get<AdwComboRow>("postprocessingThreadsRow"), postprocessingThreads);
        adw_combo_row_set_selected(m_builder.get<AdwComboRow>("postprocessingThreadsRow"), options.getPostprocessingThreads() - 1);
        adw_switch_row_set_active(m_builder.get<AdwSwitchRow>("useAriaRow"), options.getUseAria());
        adw_spin_row_set_value(m_builder.get<AdwSpinRow>("ariaMaxConnectionsPerServerRow"), static_cast<double>(options.getAriaMaxConnectionsPerServer()));
        adw_spin_row_set_value(m_builder.get<AdwSpinRow>("ariaMinSplitSizeRow"), static_cast<double>(options.getAriaMinSplitSize()));
        if(Environment::getDeploymentMode() != DeploymentMode::Local)
        {
            gtk_widget_set_visible(m_builder.get<GtkWidget>("cookiesBrowserRow"), false);
        }
        //Signals
        m_closed += [&](const EventArgs&) { onClosed(); };
        g_signal_connect(m_builder.get<GObject>("themeRow"), "notify::selected-item", G_CALLBACK(+[](GObject*, GParamSpec* pspec, gpointer data){ reinterpret_cast<PreferencesDialog*>(data)->onThemeChanged(); }), this);
        g_signal_connect(m_builder.get<GObject>("selectCookiesFileButton"), "clicked", G_CALLBACK(+[](GtkButton*, gpointer data){ reinterpret_cast<PreferencesDialog*>(data)->selectCookiesFile(); }), this);
        g_signal_connect(m_builder.get<GObject>("clearCookiesFileButton"), "clicked", G_CALLBACK(+[](GtkButton*, gpointer data){ reinterpret_cast<PreferencesDialog*>(data)->clearCookiesFile(); }), this);
    }

    void PreferencesDialog::onClosed()
    {
        DownloaderOptions options{ m_controller->getDownloaderOptions() };
        m_controller->setCompletedNotificationPreference(static_cast<CompletedNotificationPreference>(adw_combo_row_get_selected(m_builder.get<AdwComboRow>("completedNotificationTriggerRow"))));
        m_controller->setPreventSuspend(adw_switch_row_get_active(m_builder.get<AdwSwitchRow>("preventSuspendRow")));
        m_controller->setHistoryLengthIndex(static_cast<size_t>(adw_combo_row_get_selected(m_builder.get<AdwComboRow>("historyLengthRow"))));
        options.setOverwriteExistingFiles(adw_switch_row_get_active(m_builder.get<AdwSwitchRow>("overwriteExistingFilesRow")));
        options.setMaxNumberOfActiveDownloads(static_cast<int>(adw_spin_row_get_value(m_builder.get<AdwSpinRow>("maxNumberOfActiveDownloadsRow"))));
        options.setLimitCharacters(adw_switch_row_get_active(m_builder.get<AdwSwitchRow>("limitCharactersRow")));
        m_controller->setDownloadImmediatelyAfterValidation(adw_switch_row_get_active(m_builder.get<AdwSwitchRow>("downloadImmediatelyRow")));
        options.setIncludeAutoGeneratedSubtitles(adw_switch_row_get_active(m_builder.get<AdwSwitchRow>("includeAutoGeneratedSubtitlesRow")));
        options.setPreferredVideoCodec(static_cast<VideoCodec>(adw_combo_row_get_selected(m_builder.get<AdwComboRow>("preferredVideoCodecRow"))));
        options.setSpeedLimit(static_cast<int>(adw_spin_row_get_value(m_builder.get<AdwSpinRow>("speedLimitRow"))));
        options.setYouTubeSponsorBlock(adw_switch_row_get_active(m_builder.get<AdwSwitchRow>("sponsorBlockRow")));
        options.setProxyUrl(gtk_editable_get_text(m_builder.get<GtkEditable>("proxyUrlRow")));
        options.setCookiesBrowser(static_cast<Browser>(adw_combo_row_get_selected(m_builder.get<AdwComboRow>("cookiesBrowserRow"))));
        options.setCookiesPath(gtk_widget_get_tooltip_text(m_builder.get<GtkWidget>("cookiesFileRow")) ? gtk_widget_get_tooltip_text(m_builder.get<GtkWidget>("cookiesFileRow")) : "");
        options.setEmbedMetadata(adw_switch_row_get_active(m_builder.get<AdwSwitchRow>("embedMetadataRow")));
        options.setEmbedChapters(adw_switch_row_get_active(m_builder.get<AdwSwitchRow>("embedChaptersRow")));
        options.setEmbedSubtitles(adw_switch_row_get_active(m_builder.get<AdwSwitchRow>("embedSubtitlesRow")));
        options.setCropAudioThumbnails(adw_switch_row_get_active(m_builder.get<AdwSwitchRow>("cropAudioThumbnailRow")));
        options.setRemoveSourceData(adw_switch_row_get_active(m_builder.get<AdwSwitchRow>("removeSourceDataRow")));
        options.setPostprocessingThreads(static_cast<int>(adw_combo_row_get_selected(m_builder.get<AdwComboRow>("postprocessingThreadsRow"))) + 1);
        options.setUseAria(adw_switch_row_get_active(m_builder.get<AdwSwitchRow>("useAriaRow")));
        options.setAriaMaxConnectionsPerServer(static_cast<int>(adw_spin_row_get_value(m_builder.get<AdwSpinRow>("ariaMaxConnectionsPerServerRow"))));
        options.setAriaMinSplitSize(static_cast<int>(adw_spin_row_get_value(m_builder.get<AdwSpinRow>("ariaMinSplitSizeRow"))));
        m_controller->setDownloaderOptions(options);
        m_controller->saveConfiguration();
    }

    void PreferencesDialog::onThemeChanged()
    {
        m_controller->setTheme(static_cast<Theme>(adw_combo_row_get_selected(m_builder.get<AdwComboRow>("themeRow"))));
        switch (m_controller->getTheme())
        {
        case Theme::Light:
            adw_style_manager_set_color_scheme(adw_style_manager_get_default(), ADW_COLOR_SCHEME_FORCE_LIGHT);
            break;
        case Theme::Dark:
            adw_style_manager_set_color_scheme(adw_style_manager_get_default(), ADW_COLOR_SCHEME_FORCE_DARK);
            break;
        default:
            adw_style_manager_set_color_scheme(adw_style_manager_get_default(), ADW_COLOR_SCHEME_DEFAULT);
            break;
        }
    }

    void PreferencesDialog::selectCookiesFile()
    {
        GtkFileDialog* fileDialog{ gtk_file_dialog_new() };
        gtk_file_dialog_set_title(fileDialog, _("Select Cookies File"));
        GtkFileFilter* filter{ gtk_file_filter_new() };
        gtk_file_filter_set_name(filter, _("TXT Files (*.txt)"));
        gtk_file_filter_add_pattern(filter, "*.txt");
        gtk_file_filter_add_pattern(filter, "*.TXT");
        GListStore* filters{ g_list_store_new(gtk_file_filter_get_type()) };
        g_list_store_append(filters, G_OBJECT(filter));
        gtk_file_dialog_set_filters(fileDialog, G_LIST_MODEL(filters));
        gtk_file_dialog_open(fileDialog, m_parent, nullptr, GAsyncReadyCallback(+[](GObject* self, GAsyncResult* res, gpointer data)
        {
            GFile* file{ gtk_file_dialog_open_finish(GTK_FILE_DIALOG(self), res, nullptr) };
            if(file)
            {
                std::filesystem::path path{ g_file_get_path(file) };
                adw_action_row_set_subtitle(reinterpret_cast<Builder*>(data)->get<AdwActionRow>("cookiesFileRow"), path.filename().string().c_str());
                gtk_widget_set_tooltip_text(reinterpret_cast<Builder*>(data)->get<GtkWidget>("cookiesFileRow"), path.string().c_str());
            }
        }), &m_builder);
    }

    void PreferencesDialog::clearCookiesFile()
    {
        adw_action_row_set_subtitle(m_builder.get<AdwActionRow>("cookiesFileRow"), _("No file selected"));
        gtk_widget_set_tooltip_text(m_builder.get<GtkWidget>("cookiesFileRow"), "");
    }
}
