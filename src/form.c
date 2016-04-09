/*
 * Copyright (C) 2016 Andrea Zagli <azagli@libero.it>
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
 */

#ifdef HAVE_CONFIG_H
	#include <config.h>
#endif

#include <libzakform/libzakform.h>
#include <libzakutils/libzakutils.h>

#include "form.h"

static void zak_form_ini_provider_class_init (ZakFormIniProviderClass *class);
static void zak_form_ini_provider_init (ZakFormIniProvider *zak_form_ini_provider);
static void zak_form_iprovider_interface_init (ZakFormIProviderInterface *iface);

static void zak_form_ini_provider_set_property (GObject *object,
                               guint property_id,
                               const GValue *value,
                               GParamSpec *pspec);
static void zak_form_ini_provider_get_property (GObject *object,
                               guint property_id,
                               GValue *value,
                               GParamSpec *pspec);

static void zak_form_ini_provider_dispose (GObject *gobject);
static void zak_form_ini_provider_finalize (GObject *gobject);

static gboolean zak_form_ini_provider_load (ZakFormIProvider *provider, GPtrArray *elements);
static gboolean zak_form_ini_provider_insert (ZakFormIProvider *provider, GPtrArray *elements);
static gboolean zak_form_ini_provider_update (ZakFormIProvider *provider, GPtrArray *elements);
static gboolean zak_form_ini_provider_delete (ZakFormIProvider *provider, GPtrArray *elements);

typedef struct
	{
		GKeyFile *kfile;
		gchar *group;
	} ZakFormIniProviderPrivate;

struct _ZakFormIniProvider
{
	GObject parent_instance;
};

G_DEFINE_TYPE_WITH_CODE (ZakFormIniProvider, zak_form_ini_provider, G_TYPE_OBJECT,
						 G_ADD_PRIVATE (ZakFormIniProvider)
						 G_IMPLEMENT_INTERFACE (ZAK_FORM_TYPE_IPROVIDER,
												zak_form_iprovider_interface_init))

static void
zak_form_ini_provider_class_init (ZakFormIniProviderClass *class)
{
	GObjectClass *object_class = G_OBJECT_CLASS (class);

	object_class->set_property = zak_form_ini_provider_set_property;
	object_class->get_property = zak_form_ini_provider_get_property;
	object_class->dispose = zak_form_ini_provider_dispose;
	object_class->finalize = zak_form_ini_provider_finalize;
}

static void
zak_form_ini_provider_init (ZakFormIniProvider *zak_form_ini_provider)
{
	ZakFormIniProviderPrivate *priv = zak_form_ini_provider_get_instance_private (zak_form_ini_provider);

	priv->kfile = NULL;
	priv->group = NULL;
}

static void
zak_form_iprovider_interface_init (ZakFormIProviderInterface *iface)
{
	iface->load = zak_form_ini_provider_load;
	iface->insert = zak_form_ini_provider_insert;
	iface->update = zak_form_ini_provider_update;
	iface->delete = zak_form_ini_provider_delete;
}

/**
 * zak_form_ini_provider_new_from_gkeyfile:
 * @kfile:
 * @group:
 *
 * Returns: the newly created #ZakFormIniProvider object.
 */
ZakFormIniProvider
*zak_form_ini_provider_new_from_gkeyfile (GKeyFile *kfile, const gchar *group)
{
	ZakFormIniProvider *zak_form_ini_provider;
	ZakFormIniProviderPrivate *priv;

	zak_form_ini_provider = ZAK_FORM_INI_PROVIDER (g_object_new (zak_form_ini_provider_get_type (), NULL));

	priv = zak_form_ini_provider_get_instance_private (zak_form_ini_provider);

	priv->kfile = g_object_ref (kfile);
	priv->group = g_strdup (group);

	return zak_form_ini_provider;
}

/**
 * zak_form_ini_provider_new_from_file:
 * @filename:
 * @group:
 *
 * Returns: the newly created #ZakFormIniProvider object.
 */
ZakFormIniProvider
*zak_form_ini_provider_new_from_file (const gchar *filename, const gchar *group)
{
	GKeyFile *kfile;

	ZakFormIniProvider *zak_form_ini_provider;

	kfile = g_key_file_new ();
	g_key_file_load_from_file (kfile, filename, G_KEY_FILE_NONE, NULL);
	zak_form_ini_provider = zak_form_ini_provider_new_from_gkeyfile (kfile, group);

	return zak_form_ini_provider;
}

/* PRIVATE */
static void
zak_form_ini_provider_set_property (GObject *object,
                   guint property_id,
                   const GValue *value,
                   GParamSpec *pspec)
{
	ZakFormIniProvider *zak_form_ini_provider = (ZakFormIniProvider *)object;
	ZakFormIniProviderPrivate *priv = zak_form_ini_provider_get_instance_private (zak_form_ini_provider);

	switch (property_id)
		{
			default:
				G_OBJECT_WARN_INVALID_PROPERTY_ID (object, property_id, pspec);
				break;
		}
}

static void
zak_form_ini_provider_get_property (GObject *object,
                   guint property_id,
                   GValue *value,
                   GParamSpec *pspec)
{
	ZakFormIniProvider *zak_form_ini_provider = (ZakFormIniProvider *)object;
	ZakFormIniProviderPrivate *priv = zak_form_ini_provider_get_instance_private (zak_form_ini_provider);

	switch (property_id)
		{
			default:
				G_OBJECT_WARN_INVALID_PROPERTY_ID (object, property_id, pspec);
				break;
		}
}

static void
zak_form_ini_provider_dispose (GObject *gobject)
{
	ZakFormIniProvider *zak_form_ini_provider = (ZakFormIniProvider *)gobject;
	ZakFormIniProviderPrivate *priv = zak_form_ini_provider_get_instance_private (zak_form_ini_provider);


	GObjectClass *parent_class = g_type_class_peek_parent (G_OBJECT_GET_CLASS (gobject));
	parent_class->dispose (gobject);
}

static void
zak_form_ini_provider_finalize (GObject *gobject)
{
	ZakFormIniProvider *zak_form_ini_provider = (ZakFormIniProvider *)gobject;
	ZakFormIniProviderPrivate *priv = zak_form_ini_provider_get_instance_private (zak_form_ini_provider);


	GObjectClass *parent_class = g_type_class_peek_parent (G_OBJECT_GET_CLASS (gobject));
	parent_class->finalize (gobject);
}

static GValue
*zak_form_ini_provider_new_gvalue_from_element (ZakFormElement *element)
{
	GValue *ret;

	gchar *value;
	gchar *type;
	GHashTable *format;

	value = zak_form_element_get_value (element);
	type = zak_form_element_get_provider_type (element);
	format = zak_form_element_get_format (element);

	if (g_ascii_strcasecmp (type, "integer") == 0)
		{
			gchar *thousands_saparator;
			gdouble unformatted;

			thousands_saparator = (gchar *)g_hash_table_lookup (format, "thousands_separator");

			unformatted = zak_utils_unformat_money_full (value, thousands_saparator, NULL);

			ret = zak_utils_gvalue_new_int ((gint)unformatted);
		}
	else if (g_ascii_strcasecmp (type, "float") == 0)
		{
			gchar *thousands_saparator;
			gchar *currency_symbol;
			gdouble unformatted;

			thousands_saparator = (gchar *)g_hash_table_lookup (format, "thousands_separator");
			currency_symbol = (gchar *)g_hash_table_lookup (format, "currency_symbol");

			unformatted = zak_utils_unformat_money_full (value, thousands_saparator, currency_symbol);
			ret = zak_utils_gvalue_new_float (unformatted);
		}
	else if (g_ascii_strcasecmp (type, "string") == 0)
		{
			ret = zak_utils_gvalue_new_string (value);
		}
	else if (g_ascii_strcasecmp (type, "boolean") == 0)
		{
			ret = zak_utils_gvalue_new_boolean (g_strcmp0 (value, "TRUE") == 0);
		}
	else if (g_ascii_strcasecmp (type, "date") == 0)
		{
			GDateTime *gdt;

			gchar *datetime_format;

			datetime_format = (gchar *)g_hash_table_lookup (format, "content");
			gdt = zak_utils_get_gdatetime_from_string (value, datetime_format);

			if (gdt == NULL)
				{
					ret = zak_utils_gvalue_new_string ("");
				}
			else
				{
					ret = zak_utils_gvalue_new_string (zak_utils_gdatetime_format (gdt, "%F"));
				}

			if (gdt != NULL)
				{
					g_date_time_unref (gdt);
				}
		}
	else if (g_ascii_strcasecmp (type, "time") == 0)
		{
			GDateTime *gdt;

			gchar *datetime_format;

			datetime_format = (gchar *)g_hash_table_lookup (format, "content");
			gdt = zak_utils_get_gdatetime_from_string (value, datetime_format);

			if (gdt == NULL)
				{
					ret = zak_utils_gvalue_new_string ("");
				}
			else
				{
					ret = zak_utils_gvalue_new_string (zak_utils_gdatetime_format (gdt, "%T"));
				}

			if (gdt != NULL)
				{
					g_date_time_unref (gdt);
				}
		}
	else if (g_ascii_strcasecmp (type, "datetime") == 0)
		{
			GDateTime *gdt;

			gchar *datetime_format;

			datetime_format = (gchar *)g_hash_table_lookup (format, "content");
			gdt = zak_utils_get_gdatetime_from_string (value, datetime_format);

			if (gdt == NULL)
				{
					ret = zak_utils_gvalue_new_string ("");
				}
			else
				{
					ret = zak_utils_gvalue_new_string (zak_utils_gdatetime_format (gdt, "%F %T"));
				}

			if (gdt != NULL)
				{
					g_date_time_unref (gdt);
				}
		}

	return ret;
}

static gboolean
zak_form_ini_provider_load (ZakFormIProvider *provider, GPtrArray *elements)
{
	gboolean ret;

	GValue *value;

	ZakFormIniProviderPrivate *priv = zak_form_ini_provider_get_instance_private (ZAK_FORM_INI_PROVIDER (provider));

	ret = TRUE;

	return ret;
}

static gboolean
zak_form_ini_provider_insert (ZakFormIProvider *provider, GPtrArray *elements)
{
	gboolean ret;

	GValue *value;

	ZakFormIniProviderPrivate *priv = zak_form_ini_provider_get_instance_private (ZAK_FORM_INI_PROVIDER (provider));

	return ret;
}

static gboolean
zak_form_ini_provider_update (ZakFormIProvider *provider, GPtrArray *elements)
{
	gboolean ret;

	GValue *value;

	ZakFormIniProviderPrivate *priv = zak_form_ini_provider_get_instance_private (ZAK_FORM_INI_PROVIDER (provider));


	return ret;
}

static gboolean
zak_form_ini_provider_delete (ZakFormIProvider *provider, GPtrArray *elements)
{
	gboolean ret;

	ZakFormIniProviderPrivate *priv = zak_form_ini_provider_get_instance_private (ZAK_FORM_INI_PROVIDER (provider));


	return ret;
}
