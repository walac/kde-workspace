/*
 * Copyright (c) 2010 Ivan Cukic <ivan.cukic(at)kde.org>
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Library General Public
 * License version 2 as published by the Free Software Foundation.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Library General Public License for more details.
 *
 * You should have received a copy of the GNU Library General Public License
 * along with this library; see the file COPYING.LIB.  If not, write to
 * the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301, USA.
 */

#ifndef ACTIVITY_INFO_H
#define ACTIVITY_INFO_H

#include <QObject>
#include <QWidget>
#include <QString>
#include <QStringList>

#include <KUrl>
#include <kdemacros.h>

/**
 * This class provides info about an activity. Most methods in it
 * require a Nepomuk backend running.
 *
 * @see KActivityConsumer for info about activities
 *
 * @since 4.5
 */
class KDE_EXPORT KActivityInfo: public QObject {
    Q_OBJECT

    Q_PROPERTY(KUrl uri READ uri)
    Q_PROPERTY(KUrl resourceUri READ resourceUri)
    Q_PROPERTY(QString id READ id)
    Q_PROPERTY(QString name READ name)

public:
    /**
     * @param activityId Id of the activity
     * @returns activity info object for the specified activity. If
     * the activity with the specified ID doesn't exist, NULL is
     * returned.
     */
    static KActivityInfo * forActivity(const QString & activityId);

    /**
     * Set of predefined resource types provided for convenience
     */
    enum ResourceType {
        UnspecifiedResource = 0,
        DocumentResource    = 1, ///< @see nao:Document
        FolderResource      = 2, ///< @see nao:Folder
        ApplicationResource = 3, ///< @see nao:Application
        ContactResource     = 4, ///< @see nao:Contact
        LocationResource    = 5  ///< @see pimo:Location

    };

    /**
     * Associates the resource with the activity represented by this
     * KActivityInfo object.
     * @param resourceUrl url of the resource
     * @param resourceType type of the resource
     */
    void associateResource(const KUrl & resourceUrl, ResourceType resourceType = UnspecifiedResource);

    /**
     * Associates the resource with the activity represented by this
     * KActivityInfo object.
     * @param resourceUrl url of the resource
     * @param resourceType type of the resource
     */
    void associateResource(const KUrl & resourceUrl, const KUrl & resourceType);

    /**
     * Removes the association of this activity and the specified
     * resource.
     * @param resourceUrl url of the resource
     */
    void disassociateResource(const KUrl & resourceUrl);

    /**
     * @param resourceType type of the resource
     * @returns a list of resources of the specified type that are
     * associated with this activity. If the type is not specified,
     * all associated resources are returned,
     */
    QList < KUrl > associatedResources(ResourceType resourceType = UnspecifiedResource) const;

    /**
     * @param resourceType type of the resource
     * @returns a list of resources of the specified type that are
     * associated with this activity. If the type is not specified,
     * all associated resources are returned,
     */
    QList < KUrl > associatedResources(const KUrl & resourceType) const;

    /**
     * @returns the URI of this activity. The same URI is used by
     * activities KIO slave.
     */
    KUrl uri() const;

    /**
     * @returns the Nepomuk resource URI of this activity
     */
    KUrl resourceUri() const;

    /**
     * @returns the id of the activity
     */
    QString id() const;

    /**
     * @returns the name of the activity
     */
    QString name() const;

    /**
     * This function is provided for convenience.
     * @returns the name of the specified activity
     * @param id id of the activity
     */
    static QString name(const QString & id);

Q_SIGNALS:
    /**
     * Emitted when the activity's name is changed
     * @param newName new name of the activity
     */
    void nameChanged(const QString & newName);

private:
    explicit KActivityInfo(const QString & activityId);
    ~KActivityInfo();

    class Private;
    Private * const d;
};

#endif // ACTIVITY_INFO_H
