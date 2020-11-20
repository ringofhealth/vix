defmodule Vix.Vips.Target do
  @moduledoc """
  *NOT supported yet*
  """

  alias Vix.Type

  @behaviour Type
  @opaque t() :: reference()

  @impl Type
  def typespec do
    quote do
      unquote(__MODULE__).t()
    end
  end

  @impl Type
  def default(nil), do: :unsupported

  @impl Type
  def cast(_value, _data), do: raise("VipsTarget is not implemented yet")
end