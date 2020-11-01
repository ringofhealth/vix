#include "nif_g_param_spec.h"
#include "vix_common.h"
#include <float.h>
#include <glib-object.h>
#include <math.h>

static double clamp_double(double value) {
  if (value == INFINITY)
    return DBL_MAX;
  else if (value == -INFINITY)
    return DBL_MIN;
  else
    return value;
}

static ERL_NIF_TERM enum_details(ErlNifEnv *env, GParamSpec *pspec) {
  GParamSpecEnum *pspec_enum = G_PARAM_SPEC_ENUM(pspec);
  ERL_NIF_TERM list, tuple, enum_value_name, enum_value;
  unsigned int i;

  list = enif_make_list(env, 0);

  for (i = 0; i < pspec_enum->enum_class->n_values - 1; i++) {
    enum_value_name =
        enif_make_atom(env, pspec_enum->enum_class->values[i].value_name);
    enum_value = enif_make_int(env, pspec_enum->enum_class->values[i].value);
    tuple = enif_make_tuple2(env, enum_value_name, enum_value);
    list = enif_make_list_cell(env, tuple, list);
  }

  return enif_make_tuple2(env, list,
                          enif_make_int(env, pspec_enum->default_value));
}

static ERL_NIF_TERM boolean_details(ErlNifEnv *env, GParamSpec *pspec) {
  GParamSpecBoolean *pspec_bool = G_PARAM_SPEC_BOOLEAN(pspec);

  if (pspec_bool->default_value) {
    return enif_make_atom(env, "true");
  } else {
    return enif_make_atom(env, "false");
  }
}

static ERL_NIF_TERM uint64_details(ErlNifEnv *env, GParamSpec *pspec) {
  GParamSpecUInt64 *pspec_uint64 = G_PARAM_SPEC_UINT64(pspec);

  return enif_make_tuple3(env, enif_make_uint64(env, pspec_uint64->minimum),
                          enif_make_uint64(env, pspec_uint64->maximum),
                          enif_make_uint64(env, pspec_uint64->default_value));
}

static ERL_NIF_TERM double_details(ErlNifEnv *env, GParamSpec *pspec) {
  GParamSpecDouble *pspec_double = G_PARAM_SPEC_DOUBLE(pspec);
  return enif_make_tuple3(
      env, enif_make_double(env, clamp_double(pspec_double->minimum)),
      enif_make_double(env, clamp_double(pspec_double->maximum)),
      enif_make_double(env, clamp_double(pspec_double->default_value)));
}

static ERL_NIF_TERM int_details(ErlNifEnv *env, GParamSpec *pspec) {
  GParamSpecInt *pspec_int = G_PARAM_SPEC_INT(pspec);
  return enif_make_tuple3(env, enif_make_int(env, pspec_int->minimum),
                          enif_make_int(env, pspec_int->maximum),
                          enif_make_int(env, pspec_int->default_value));
}

static ERL_NIF_TERM uint_details(ErlNifEnv *env, GParamSpec *pspec) {
  GParamSpecUInt *pspec_uint = G_PARAM_SPEC_UINT(pspec);
  return enif_make_tuple3(env, enif_make_uint(env, pspec_uint->minimum),
                          enif_make_uint(env, pspec_uint->maximum),
                          enif_make_uint(env, pspec_uint->default_value));
}

static ERL_NIF_TERM int64_details(ErlNifEnv *env, GParamSpec *pspec) {
  GParamSpecInt64 *pspec_int64 = G_PARAM_SPEC_INT64(pspec);
  return enif_make_tuple3(env, enif_make_int64(env, pspec_int64->minimum),
                          enif_make_int64(env, pspec_int64->maximum),
                          enif_make_int64(env, pspec_int64->default_value));
}

static ERL_NIF_TERM string_details(ErlNifEnv *env, GParamSpec *pspec) {
  GParamSpecString *pspec_string = G_PARAM_SPEC_STRING(pspec);
  if (pspec_string->default_value == NULL) {
    return enif_make_string(env, "", ERL_NIF_LATIN1);
  } else {
    return enif_make_string(env, pspec_string->default_value, ERL_NIF_LATIN1);
  }
}

static ERL_NIF_TERM flag_details(ErlNifEnv *env, GParamSpec *pspec) {

  GParamSpecFlags *pspec_flags = G_PARAM_SPEC_FLAGS(pspec);
  ERL_NIF_TERM list, tuple, flag_name, flag_value;
  unsigned int i;

  list = enif_make_list(env, 0);

  for (i = 0; i < pspec_flags->flags_class->n_values - 1; i++) {
    flag_name =
        enif_make_atom(env, pspec_flags->flags_class->values[i].value_name);
    flag_value = enif_make_int(env, pspec_flags->flags_class->values[i].value);

    tuple = enif_make_tuple2(env, flag_name, flag_value);
    list = enif_make_list_cell(env, tuple, list);
  }

  return enif_make_tuple2(env, list,
                          enif_make_int(env, pspec_flags->default_value));
}

ERL_NIF_TERM g_param_spec_to_erl_term(ErlNifEnv *env, GParamSpec *pspec) {
  GParamSpecResource *pspec_r =
      enif_alloc_resource(G_PARAM_SPEC_RT, sizeof(GParamSpecResource));

  pspec_r->pspec = pspec;
  ERL_NIF_TERM term = enif_make_resource(env, pspec_r);
  enif_release_resource(pspec_r);

  return term;
}

bool erl_term_to_g_param_spec(ErlNifEnv *env, ERL_NIF_TERM term,
                              GParamSpec **pspec) {
  GParamSpecResource *pspec_r = NULL;
  if (enif_get_resource(env, term, G_PARAM_SPEC_RT, (void **)&pspec_r)) {
    (*pspec) = pspec_r->pspec;
    return true;
  } else {
    return false;
  }
}

ERL_NIF_TERM g_param_spec_details(ErlNifEnv *env, GParamSpec *pspec) {
  ERL_NIF_TERM term, value_type, spec_type;

  spec_type = enif_make_string(env, g_type_name(G_PARAM_SPEC_TYPE(pspec)),
                               ERL_NIF_LATIN1);

  value_type = enif_make_string(
      env, g_type_name(G_PARAM_SPEC_VALUE_TYPE(pspec)), ERL_NIF_LATIN1);

  if (G_IS_PARAM_SPEC_ENUM(pspec)) {
    term = enum_details(env, pspec);
  } else if (G_IS_PARAM_SPEC_BOOLEAN(pspec)) {
    term = boolean_details(env, pspec);
  } else if (G_IS_PARAM_SPEC_UINT64(pspec)) {
    term = uint64_details(env, pspec);
  } else if (G_IS_PARAM_SPEC_DOUBLE(pspec)) {
    term = double_details(env, pspec);
  } else if (G_IS_PARAM_SPEC_INT(pspec)) {
    term = int_details(env, pspec);
  } else if (G_IS_PARAM_SPEC_UINT(pspec)) {
    term = uint_details(env, pspec);
  } else if (G_IS_PARAM_SPEC_INT64(pspec)) {
    term = int64_details(env, pspec);
  } else if (G_IS_PARAM_SPEC_STRING(pspec)) {
    term = string_details(env, pspec);
  } else if (G_IS_PARAM_SPEC_FLAGS(pspec)) {
    term = flag_details(env, pspec);
  } else if (G_IS_PARAM_SPEC_BOXED(pspec)) {
    term = enif_make_atom(env, "nil");
  } else if (G_IS_PARAM_SPEC_OBJECT(pspec)) {
    term = enif_make_atom(env, "nil");
  } else {
    error("Unknown GParamSpec: %s",
          g_type_name(G_PARAM_SPEC_VALUE_TYPE(pspec)));
    return enif_make_badarg(env);
  }

  return enif_make_tuple3(env, spec_type, value_type, term);
}

/******* GParamSpec Resource *******/
static void g_param_spec_dtor(ErlNifEnv *env, void *obj) {
  debug("GParamSpec g_param_spec_dtor called");
}

static void g_param_spec_stop(ErlNifEnv *env, void *obj, int fd,
                              int is_direct_call) {
  debug("GParamSpec g_param_spec_stop called %d", fd);
}

static void g_param_spec_down(ErlNifEnv *env, void *obj, ErlNifPid *pid,
                              ErlNifMonitor *monitor) {
  debug("GParamSpec g_param_spec_down called");
}

static ErlNifResourceTypeInit g_param_spec_rt_init = {
    g_param_spec_dtor, g_param_spec_stop, g_param_spec_down};

int nif_g_param_spec_init(ErlNifEnv *env) {

  G_PARAM_SPEC_RT = enif_open_resource_type_x(
      env, "g_param_spec_resource", &g_param_spec_rt_init,
      ERL_NIF_RT_CREATE | ERL_NIF_RT_TAKEOVER, NULL);
  return 0;
}
