//
// Aspia Project
// Copyright (C) 2020 Dmitry Chapyshev <dmitry@aspia.ru>
//
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program. If not, see <https://www.gnu.org/licenses/>.
//

#include "client/ui/client_dialog.h"

#include "base/logging.h"
#include "base/net/address.h"
#include "build/build_config.h"
#include "client/config_factory.h"
#include "client/ui/client_settings.h"
#include "client/ui/desktop_config_dialog.h"
#include "client/ui/qt_desktop_window.h"
#include "client/ui/qt_file_manager_window.h"
#include "common/desktop_session_constants.h"
#include "common/session_type.h"
#include "ui_client_dialog.h"

#include <QMessageBox>

namespace client {

ClientDialog::ClientDialog(const RouterList& routers, QWidget* parent)
    : QDialog(parent),
      ui(std::make_unique<Ui::ClientDialog>()),
      routers_(routers)
{
    config_.port = DEFAULT_HOST_TCP_PORT;
    config_.session_type = proto::SESSION_TYPE_DESKTOP_MANAGE;
    desktop_config_ = ConfigFactory::defaultDesktopManageConfig();

    ui->setupUi(this);
    setFixedHeight(sizeHint().height());

    QComboBox* combo_router = ui->combo_router;
    combo_router->addItem(tr("Without Router"), -1);

    for (size_t i = 0; i < routers.size(); ++i)
        combo_router->addItem(QString::fromStdU16String(routers[i].name), static_cast<int>(i));

    if (routers.empty())
        combo_router->setCurrentIndex(0);

    connect(combo_router, QOverload<int>::of(&QComboBox::currentIndexChanged), [this](int index)
    {
        if (ui->combo_router->itemData(index).toInt() == -1)
        {
            ui->label_address->setText(tr("Address:"));
        }
        else
        {
            ui->label_address->setText("ID:");
        }
    });

    QComboBox* combo_address = ui->combo_address;

    ClientSettings settings;
    combo_address->addItems(settings.addressList());
    combo_address->setCurrentIndex(0);

    auto add_session = [this](const QString& icon, proto::SessionType session_type)
    {
        ui->combo_session_type->addItem(QIcon(icon),
                                        common::sessionTypeToLocalizedString(session_type),
                                        QVariant(session_type));
    };

    add_session(":/img/monitor-keyboard.png", proto::SESSION_TYPE_DESKTOP_MANAGE);
    add_session(":/img/monitor.png", proto::SESSION_TYPE_DESKTOP_VIEW);
    add_session(":/img/folder-stand.png", proto::SESSION_TYPE_FILE_TRANSFER);

    int current_session_type = ui->combo_session_type->findData(QVariant(config_.session_type));
    if (current_session_type != -1)
    {
        ui->combo_session_type->setCurrentIndex(current_session_type);
        sessionTypeChanged(current_session_type);
    }

    connect(ui->button_clear, &QPushButton::released, [this]()
    {
        int ret = QMessageBox::question(
            this,
            tr("Confirmation"),
            tr("The list of entered addresses will be cleared. Continue?"),
            QMessageBox::Yes | QMessageBox::No);

        if (ret == QMessageBox::Yes)
        {
            ui->combo_address->clear();

            ClientSettings settings;
            settings.setAddressList(QStringList());
        }
    });

    connect(ui->combo_session_type, QOverload<int>::of(&QComboBox::currentIndexChanged),
            this, &ClientDialog::sessionTypeChanged);

    connect(ui->button_session_config, &QPushButton::released,
            this, &ClientDialog::sessionConfigButtonPressed);

    connect(ui->button_box, &QDialogButtonBox::clicked, this, &ClientDialog::onButtonBoxClicked);

    combo_address->setFocus();
}

ClientDialog::~ClientDialog() = default;

void ClientDialog::sessionTypeChanged(int item_index)
{
    proto::SessionType session_type = static_cast<proto::SessionType>(
        ui->combo_session_type->itemData(item_index).toInt());

    switch (session_type)
    {
        case proto::SESSION_TYPE_DESKTOP_MANAGE:
        {
            ui->button_session_config->setEnabled(true);
            desktop_config_ = ConfigFactory::defaultDesktopManageConfig();
        }
        break;

        case proto::SESSION_TYPE_DESKTOP_VIEW:
        {
            ui->button_session_config->setEnabled(true);
            desktop_config_ = ConfigFactory::defaultDesktopViewConfig();
        }
        break;

        default:
            ui->button_session_config->setEnabled(false);
            break;
    }
}

void ClientDialog::sessionConfigButtonPressed()
{
    proto::SessionType session_type = static_cast<proto::SessionType>(
        ui->combo_session_type->currentData().toInt());

    switch (session_type)
    {
        case proto::SESSION_TYPE_DESKTOP_MANAGE:
        case proto::SESSION_TYPE_DESKTOP_VIEW:
        {
            DesktopConfigDialog dialog(session_type,
                                       desktop_config_,
                                       common::kSupportedVideoEncodings,
                                       this);

            if (dialog.exec() == DesktopConfigDialog::Accepted)
                desktop_config_ = dialog.config();
        }
        break;

        default:
            break;
    }
}

void ClientDialog::onButtonBoxClicked(QAbstractButton* button)
{
    if (ui->button_box->standardButton(button) == QDialogButtonBox::Cancel)
    {
        reject();
        close();
        return;
    }

    std::optional<client::RouterConfig> router_config;

    int current_router = ui->combo_router->currentIndex();
    if (current_router != -1)
    {
        size_t current_router_index =
            static_cast<size_t>(ui->combo_router->itemData(current_router).toInt());

        if (current_router_index < routers_.size())
        {
            router_config.emplace(routers_[current_router_index]);
        }
        else
        {
            LOG(LS_ERROR) << "Invalid router index: " << current_router_index;
        }
    }

    QComboBox* combo_address = ui->combo_address;
    QString current_address = combo_address->currentText();

    if (!router_config.has_value())
    {
        LOG(LS_INFO) << "Direct connection selected";

        base::Address address = base::Address::fromString(
            current_address.toStdU16String(), DEFAULT_HOST_TCP_PORT);

        if (!address.isValid())
        {
            QMessageBox::warning(this,
                                 tr("Warning"),
                                 tr("An invalid computer address was entered."),
                                 QMessageBox::Ok);
            combo_address->setFocus();
            return;
        }

        config_.address_or_id = address.host();
        config_.port = address.port();
    }
    else
    {
        LOG(LS_INFO) << "Relay connection selected";

        config_.address_or_id = current_address.toStdU16String();
        config_.username = u"#" + config_.address_or_id;
    }

    int current_index = combo_address->findText(current_address);
    if (current_index != -1)
        combo_address->removeItem(current_index);

    combo_address->insertItem(0, current_address);
    combo_address->setCurrentIndex(0);

    QStringList address_list;
    for (int i = 0; i < std::min(combo_address->count(), 15); ++i)
        address_list.append(combo_address->itemText(i));

    ClientSettings settings;
    settings.setAddressList(address_list);

    proto::SessionType session_type = static_cast<proto::SessionType>(
        ui->combo_session_type->currentData().toInt());

    if (router_config.has_value())
        config_.router_config = std::move(router_config.value());

    config_.session_type = session_type;

    ClientWindow* client_window = nullptr;

    switch (config_.session_type)
    {
        case proto::SESSION_TYPE_DESKTOP_MANAGE:
        case proto::SESSION_TYPE_DESKTOP_VIEW:
        {
            client_window = new QtDesktopWindow(
                config_.session_type, desktop_config_, parentWidget());
        }
        break;

        case proto::SESSION_TYPE_FILE_TRANSFER:
            client_window = new client::QtFileManagerWindow(parentWidget());
            break;

        default:
            NOTREACHED();
            break;
    }

    if (!client_window)
        return;

    client_window->setAttribute(Qt::WA_DeleteOnClose);
    if (!client_window->connectToHost(config_))
    {
        client_window->close();
    }
    else
    {
        accept();
        close();
    }
}

} // namespace client
