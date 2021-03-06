external internalQuery : Js.t {..} => Js.t {..} =
  "query" [@@bs.module "@phenomic/api-client/lib/query"];

type reasonChildren = list reasonChild
and reasonChild =
  | String string
  | Element string (Js.t {.}) reasonChildren
  | Empty;

type jsBody = Js.t {. t : string, p : Js.t {.}, c : Js.Null_undefined.t (array jsBody)};

let rec jsTreeToReason (jsChild: jsBody) =>
  switch [%bs.raw {| Object.prototype.toString.call(jsChild) |}] {
  | "[object String]" => String (Js.String.make jsChild)
  | "[object Object]" =>
    let tag = Js.String.make jsChild##t;
    let props = jsChild##p;
    let children =
      switch (Js.Null_undefined.to_opt jsChild##c) {
      | Some c => List.map jsTreeToReason (Array.to_list c)
      | None => []
      };
    Element tag props children
  | _ => Empty
  };

type edge 'a =
  | Idle 'a
  | Loading
  | Inactive
  | Errored;

type jsEdge 'a = Js.t {. status : string, node : 'a};

let jsEdgeToReason jsEdge convertNode =>
  switch jsEdge##status {
  | "loading" => Loading
  | "errored" => Errored
  | "idle" => Idle (convertNode jsEdge##node)
  | "inactive"
  | _ => Inactive
  };

type queryConfigItem = {
  path: string,
  id: string
};

type listConfig = {
  path: string,
  by: option string,
  value: option string,
  order: option string,
  limit: option int
};

type paginatedListConfig = {
  path: string,
  by: option string,
  value: option string,
  order: option string,
  limit: option int,
  after: option string
};

type queryConfig =
  | Item queryConfigItem
  | List listConfig
  | PaginatedList paginatedListConfig;

let query queryConfig =>
  switch queryConfig {
  | Item queryConfigItem =>
    Js.Obj.assign (Js.Obj.empty ()) {"path": queryConfigItem.path, "id": queryConfigItem.id}
  | List queryConfigList =>
    Js.Obj.assign
      (Js.Obj.empty ())
      {
        "path": queryConfigList.path,
        "by": Js.Null_undefined.from_opt queryConfigList.by,
        "value": Js.Null_undefined.from_opt queryConfigList.value,
        "order": Js.Null_undefined.from_opt queryConfigList.order,
        "limit": Js.Null_undefined.from_opt queryConfigList.limit
      }
  | PaginatedList queryConfigPaginatedList =>
    Js.Obj.assign
      (Js.Obj.empty ())
      {
        "path": queryConfigPaginatedList.path,
        "by": Js.Null_undefined.from_opt queryConfigPaginatedList.by,
        "value": Js.Null_undefined.from_opt queryConfigPaginatedList.value,
        "order": Js.Null_undefined.from_opt queryConfigPaginatedList.order,
        "limit": Js.Null_undefined.from_opt queryConfigPaginatedList.limit,
        "after": Js.Null_undefined.from_opt queryConfigPaginatedList.after
      }
  };
