class PedidosWindow extends Base_Window {
  
  // Declaration of class members
  menu_bar = null;
  menu_pedido_nuevo = null;
  menu_pedido_guardar = null;
  menu_pedido_borrar = null;
  menu_pedido_imprimir = null;
  menu_pedidos_mostrar_existentes = null;
  menu_pedidos_modificar_existente = null;
  menu_pedidos_borrar_existente = null;
  menu_pedidos_imprimir_selecionados = null;
  menu_pedidos_imprimir_todos = null;
  menu_clientes_mostrar_existentes = null;
  menu_clientes_nuevo = null;
  menu_clientes_guardar = null;
  menu_clientes_borrar = null;
  menu_clientes_importar = null;
  menu_clientes_exportar = null;
  menu_articulos_mostrar_existentes = null;
  menu_articulos_nuevo = null;
  menu_articulos_guardar = null;
  menu_articulos_borrar = null;
  menu_articulos_importar = null;
  menu_articulos_exportar = null;
  menu_totales_mostrar_existentes = null;
  menu_articulos_imprimir = null;
  menu_preguntar_por_impresora = null;
  menu_imprimir_papel_A4 = null;
  menu_imprimir_papel_A5 = null;
  theme_default = null;
  theme_gtk = null;
  theme_plastic = null;
  menu_salir = null;
  tabs = null;
  tab_pedido = null;
  grupo_pedido_cliente = null;
  pedidos_cliente_codigo_r = null;
  pedidos_numero_r = null;
  pedidos_fecha_r = null;
  pedidos_cliente_nombre_r = null;
  grid_pedido_lineas = null;
  grupo_pedido_lineas = null;
  pedido_lineas_descripcion = null;
  pedido_lineas_codigo_r = null;
  pedido_lineas_bultos = null;
  pedido_lineas_envase = null;
  pedido_lineas_peso = null;
  pedido_lineas_cantidad = null;
  btnImprimirPedido = null;
  lines_count = null;
  tab_pedidos_lista = null;
  grid_pedidos = null;
  btnActualizarPedidos = null;
  btnImprimirTodosPedidos = null;
  btnExportarPedidos = null;
  btnImportarPedidos = null;
  btnBorrarTodosPedidos = null;
  tab_clientes_lista = null;
  grid_clientes = null;
  editClienteGroup = null;
  clientes_nombre = null;
  clientes_codigo = null;
  clientes_telefono = null;
  clientes_activo = null;
  btnGuardarCliente = null;
  clientes_observaciones = null;
  btnActualizarClientes = null;
  btnImportarClientes = null;
  tab_articulos_lista = null;
  grid_articulos = null;
  articuloEditGroup = null;
  articulos_descripcion = null;
  articulos_codigo = null;
  articulos_activo = null;
  btnGuardarArticulo = null;
  articulos_observaciones = null;
  articulos_use_peso = null;
  articulos_use_bultos = null;
  btnActualizarArticulos = null;
  btnImportarArticulos = null;
  tab_totales = null;
  grid_totales = null;
  grid_detalle_totales = null;
  btnActualizarTotales = null;
  btnImprimirTotales = null;
  btnCrearCierreTotales = null;
  tab_opciones = null;
  grupo_opciones_impresora = null;
  preguntar_por_impresora = null;
  imprimir_papel_A4 = null;
  imprimir_papel_A5 = null;
  btnFechaTrabajo = null;
  fecha_de_trabajo = null;
  lista_de_envases = null;
  btnGuardarEnvases = null;
  status_bar = null;
  
  constructor(px=74, py=62, pw=800, ph=560, pl=_tr("Pedidos")){
    base.constructor(px, py, pw, ph, pl);
    // Create member functions and widgets
    {
      local o = Fl_Menu_Bar(0, 0, 800, 25);
      menu_bar = o;
      o.textsize(16);
      o.labelsize(16);
      {
        //menu_pedido &Pedido (F2)
        menu_pedido_nuevo = o.add(_tr("&Pedido (F2)/Crear un nuevo Pedido"), 262254);
        menu_pedido_guardar = o.add(_tr("&Pedido (F2)/Guardar el Pedido"), 0, null, null, 1);
        menu_pedido_borrar = o.add(_tr("&Pedido (F2)/Borrar el  Pedido"), 0, null, null, 129);
        menu_pedido_imprimir = o.add(_tr("&Pedido (F2)/Imprimir el Pedido"), 0, null, null, 1);
        //menu_pedidos Pedidos (F3)
        menu_pedidos_mostrar_existentes = o.add(_tr("Pedidos (F3)/Listado de Pedidos existentes"), 0);
        menu_pedidos_modificar_existente = o.add(_tr("Pedidos (F3)/Modificar el Pedido selecionado"), 0);
        menu_pedidos_borrar_existente = o.add(_tr("Pedidos (F3)/Borrar el Pedido selecionado"), 0, null, null, 129);
        menu_pedidos_imprimir_selecionados = o.add(_tr("Pedidos (F3)/Imprimir los Pedidos selecionados"), 0);
        menu_pedidos_imprimir_todos = o.add(_tr("Pedidos (F3)/Imprimir todos pedidos aun no impresos"), 0);
        //menu_clientes &Clientes (F4)
        menu_clientes_mostrar_existentes = o.add(_tr("&Clientes (F4)/Listado de clientes existentes"), 0);
        menu_clientes_nuevo = o.add(_tr("&Clientes (F4)/Añadir un nuevo cliente"), 0);
        menu_clientes_guardar = o.add(_tr("&Clientes (F4)/Modificar un cliente existente"), 0, null, null, 1);
        menu_clientes_borrar = o.add(_tr("&Clientes (F4)/Borrar un cliente existente"), 0, null, null, 129);
        menu_clientes_importar = o.add(_tr("&Clientes (F4)/Importar clientes"), 0);
        menu_clientes_exportar = o.add(_tr("&Clientes (F4)/Exportar clientes"), 0);
        //menu_articulos &Artículos (F5)
        menu_articulos_mostrar_existentes = o.add(_tr("&Artículos (F5)/Listado de articulos existentes"), 0);
        menu_articulos_nuevo = o.add(_tr("&Artículos (F5)/Añadir un nuevo artículo"), 0);
        menu_articulos_guardar = o.add(_tr("&Artículos (F5)/Modificar un artículo existente"), 0, null, null, 1);
        menu_articulos_borrar = o.add(_tr("&Artículos (F5)/Borrar un artículo existente"), 0, null, null, 129);
        menu_articulos_importar = o.add(_tr("&Artículos (F5)/Importar artículos"), 0);
        menu_articulos_exportar = o.add(_tr("&Artículos (F5)/Exportar artículos"), 0);
        //menu_totales &Totales (F6)
        menu_totales_mostrar_existentes = o.add(_tr("&Totales (F6)/Listado de totales existentes"), 0);
        menu_articulos_imprimir = o.add(_tr("&Totales (F6)/Exportar totales"), 0);
        //menu_opciones &Opciones (F9)
        menu_preguntar_por_impresora = o.add(_tr("&Opciones (F9)/Preguntar por impresora"), 0, null, null, 130);
        menu_imprimir_papel_A4 = o.add(_tr("&Opciones (F9)/Imprimir en papel A4"), 0, null, null, 12);
        menu_imprimir_papel_A5 = o.add(_tr("&Opciones (F9)/Imprimir en papel A5"), 0, null, null, 136);
        theme_default = o.add(_tr("&Opciones (F9)/Tema por defecto"), 0, null, null, 12);
        theme_gtk = o.add(_tr("&Opciones (F9)/Tema GTK+"), 0, null, null, 8);
        theme_plastic = o.add(_tr("&Opciones (F9)/Tema Plastico"), 0, null, null, 8);
        menu_salir = o.add(_tr("&Salir"), 0);
      }
    }
    {
      local o = Fl_Tabs(0, 25, 800, 504);
      tabs = o;
      o.labelcolor(7);
      o.labeltype(FL_NO_LABEL);
      o.labelsize(16);
      {
        {
          local o = Fl_Group(0, 25, 800, 504, _tr("Pedido (F2)"));
          tab_pedido = o;
          o.labelsize(16);
          o.labeltype(FL_NO_LABEL);
          {
            {
              local o = Fl_Group(10, 47, 780, 88, _tr("@2 Cliente @2"));
              grupo_pedido_cliente = o;
              o.labelcolor(66);
              o.labelsize(16);
              o.color(246);
              o.box(FL_ENGRAVED_BOX);
              {
                {
                  local o = Fl_Input(91, 59, 100, 28, _tr("Código"));
                  pedidos_cliente_codigo_r = o;
                  o.textsize(16);
                  o.labelsize(16);
                  add_input_field_to_map("pedidos", "cliente_codigo", pedidos_cliente_codigo_r);
                }
                {
                  local o = Fl_Output(425, 59, 134, 28, _tr("Número"));
                  pedidos_numero_r = o;
                  o.textsize(16);
                  o.labelsize(16);
                  add_input_field_to_map("pedidos", "numero", pedidos_numero_r);
                }
                {
                  local o = Fl_Output(646, 59, 134, 28, _tr("Fecha"));
                  pedidos_fecha_r = o;
                  o.textsize(16);
                  o.labelsize(16);
                  add_input_field_to_map("pedidos", "fecha", pedidos_fecha_r);
                }
                {
                  local o = Fl_Input(91, 97, 694, 28, _tr("Nombre"));
                  pedidos_cliente_nombre_r = o;
                  o.textsize(16);
                  o.labelsize(16);
                  add_input_field_to_map("pedidos", "cliente_nombre", pedidos_cliente_nombre_r);
                }
              }
              o.end();
            }
            {
              local o = Fl_Data_Table(10, 145, 780, 230, _tr("@2 Lineas del Pedido @2"));
              grid_pedido_lineas = o;
              o.textsize(16);
              o.align(1);
              o.box(FL_ENGRAVED_BOX);
              o.color(175);
              o.labelsize(16);
              o.end();
              Fl_Group.current().resizable(o);
            }
            {
              local o = Fl_Group(10, 403, 780, 121, _tr("@2 Añadir / Editar / Borar Artículo @2"));
              grupo_pedido_lineas = o;
              o.labelcolor(57);
              o.labelsize(16);
              o.color(167);
              o.box(FL_ENGRAVED_BOX);
              {
                {
                  local o = Fl_Input(24, 430, 756, 28, _tr("Descripcion"));
                  pedido_lineas_descripcion = o;
                  o.textsize(16);
                  o.align(5);
                  o.labelsize(16);
                  add_input_field_to_map("pedido_lineas", "descripcion", pedido_lineas_descripcion);
                }
                {
                  local o = Fl_Input(24, 486, 100, 28, _tr("Código"));
                  pedido_lineas_codigo_r = o;
                  o.textsize(16);
                  o.align(5);
                  o.when(1);
                  o.tooltip(_tr("Para un nuevo pedido pulsar +"));
                  o.labelsize(16);
                  add_input_field_to_map("pedido_lineas", "codigo", pedido_lineas_codigo_r);
                }
                {
                  local o = Fl_Float_Input(134, 486, 90, 28, _tr("Bultos"));
                  pedido_lineas_bultos = o;
                  o.textsize(16);
                  o.align(5);
                  o.labelsize(16);
                  add_input_field_to_map("pedido_lineas", "bultos", pedido_lineas_bultos);
                }
                {
                  local o = Fl_Choice(234, 486, 141, 28, _tr("Envase"));
                  pedido_lineas_envase = o;
                  o.textsize(16);
                  o.align(5);
                  o.down_box(FL_BORDER_BOX);
                  o.labelsize(16);
                  add_input_field_to_map("pedido_lineas", "envase", pedido_lineas_envase);
                }
                {
                  local o = Fl_Float_Input(385, 486, 90, 28, _tr("Kilos"));
                  pedido_lineas_peso = o;
                  o.textsize(16);
                  o.align(5);
                  o.labelsize(16);
                  add_input_field_to_map("pedido_lineas", "peso", pedido_lineas_peso);
                }
                {
                  local o = Fl_Float_Input(485, 486, 90, 28, _tr("Unidades"));
                  pedido_lineas_cantidad = o;
                  o.textsize(16);
                  o.align(5);
                  o.labelsize(16);
                  add_input_field_to_map("pedido_lineas", "cantidad", pedido_lineas_cantidad);
                }
                {
                  local o = Fl_Button(660, 486, 120, 28, _tr("&Imprimir"));
                  btnImprimirPedido = o;
                  o.labelsize(16);
                }
              }
              o.end();
            }
            {
              local o = Fl_Output(20, 374, 30, 28, _tr("@<- Lineas"));
              lines_count = o;
              o.textsize(16);
              o.align(8);
              o.box(FL_NO_BOX);
              o.labelsize(16);
            }
          }
          o.end();
        }
        {
          local o = Fl_Group(0, 25, 800, 504, _tr("Pedidos (F3)"));
          tab_pedidos_lista = o;
          o.labelsize(16);
          o.labeltype(FL_NO_LABEL);
          {
            {
              local o = Fl_Data_Table(10, 38, 780, 405, _tr("@2 Lista de Pedidos @2"));
              grid_pedidos = o;
              o.textsize(16);
              o.align(17);
              o.tooltip(_tr("Para ver o editar un pedido seleccionar y presionar la barra de espacio"));
              o.box(FL_ENGRAVED_BOX);
              o.labelsize(16);
              o.color(175);
              o.end();
              Fl_Group.current().resizable(o);
            }
            {
              local o = Fl_Button(10, 453, 120, 28, _tr("Actualizar"));
              btnActualizarPedidos = o;
              o.labelsize(16);
            }
            {
              local o = Fl_Button(140, 453, 145, 28, _tr("Imprimir Todos"));
              btnImprimirTodosPedidos = o;
              o.labelsize(16);
            }
            {
              local o = Fl_Button(295, 453, 120, 28, _tr("Exportar"));
              btnExportarPedidos = o;
              o.labelsize(16);
            }
            {
              local o = Fl_Button(425, 453, 120, 28, _tr("Importar"));
              btnImportarPedidos = o;
              o.labelsize(16);
            }
            {
              local o = Fl_Button(670, 453, 120, 28, _tr("Borrar Todos"));
              btnBorrarTodosPedidos = o;
              o.labelcolor(7);
              o.color(90);
              o.labelsize(16);
            }
          }
          o.end();
        }
        {
          local o = Fl_Group(0, 25, 800, 504, _tr("Clientes (F4)"));
          tab_clientes_lista = o;
          o.labelsize(16);
          o.labeltype(FL_NO_LABEL);
          {
            {
              local o = Fl_Data_Table(10, 38, 780, 249, _tr("@2 Lista de Clientes @2"));
              grid_clientes = o;
              o.textsize(16);
              o.align(17);
              o.box(FL_ENGRAVED_BOX);
              o.color(175);
              o.labelsize(16);
              o.end();
              Fl_Group.current().resizable(o);
            }
            {
              local o = Fl_Group(10, 336, 780, 185, _tr("@2 Datos del Cliente @2"));
              editClienteGroup = o;
              o.align(5);
              o.box(FL_ENGRAVED_BOX);
              o.color(246);
              o.labelsize(16);
              o.labelcolor(66);
              {
                {
                  local o = Fl_Input(91, 348, 684, 28, _tr("Nombre"));
                  clientes_nombre = o;
                  o.textsize(16);
                  o.labelsize(16);
                  add_input_field_to_map("clientes", "nombre", clientes_nombre);
                }
                {
                  local o = Fl_Input(91, 386, 100, 28, _tr("Código"));
                  clientes_codigo = o;
                  o.textsize(16);
                  o.labelsize(16);
                  add_input_field_to_map("clientes", "codigo", clientes_codigo);
                }
                {
                  local o = Fl_Input(285, 386, 100, 28, _tr("Teléfono"));
                  clientes_telefono = o;
                  o.textsize(16);
                  o.labelsize(16);
                  add_input_field_to_map("clientes", "telefono", clientes_telefono);
                }
                {
                  local o = Fl_Check_Button(405, 390, 82, 20, _tr("Activo"));
                  clientes_activo = o;
                  o.labelsize(16);
                  o.down_box(FL_DOWN_BOX);
                  add_input_field_to_map("clientes", "activo", clientes_activo);
                }
                {
                  local o = Fl_Button(655, 386, 120, 28, _tr("Guardar"));
                  btnGuardarCliente = o;
                  o.labelsize(16);
                }
                {
                  local o = Fl_Multiline_Input(90, 422, 685, 86, _tr("Obs."));
                  clientes_observaciones = o;
                  o.textsize(16);
                  o.align(7);
                  o.labelsize(16);
                  add_input_field_to_map("clientes", "observaciones", clientes_observaciones);
                }
              }
              o.end();
            }
            {
              local o = Fl_Button(540, 300, 120, 28, _tr("Actualizar"));
              btnActualizarClientes = o;
              o.labelsize(16);
            }
            {
              local o = Fl_Button(670, 300, 120, 28, _tr("Importar"));
              btnImportarClientes = o;
              o.labelsize(16);
            }
          }
          o.end();
        }
        {
          local o = Fl_Group(0, 25, 800, 504, _tr("Artículos (F5)"));
          tab_articulos_lista = o;
          o.labelsize(15);
          o.labeltype(FL_NO_LABEL);
          {
            {
              local o = Fl_Data_Table(10, 38, 780, 254, _tr("@2 Lista de Artículos @2"));
              grid_articulos = o;
              o.textsize(16);
              o.align(17);
              o.box(FL_ENGRAVED_BOX);
              o.color(175);
              o.labelsize(16);
              o.end();
              Fl_Group.current().resizable(o);
            }
            {
              local o = Fl_Group(10, 338, 780, 183, _tr("@2 Datos del Artículo @2"));
              articuloEditGroup = o;
              o.align(5);
              o.box(FL_ENGRAVED_BOX);
              o.color(246);
              o.labelsize(16);
              o.labelcolor(66);
              {
                {
                  local o = Fl_Input(108, 348, 672, 28, _tr("Descripción"));
                  articulos_descripcion = o;
                  o.textsize(16);
                  o.labelsize(16);
                  add_input_field_to_map("articulos", "descripcion", articulos_descripcion);
                }
                {
                  local o = Fl_Input(90, 386, 100, 28, _tr("Código"));
                  articulos_codigo = o;
                  o.textsize(16);
                  o.labelsize(16);
                  add_input_field_to_map("articulos", "codigo", articulos_codigo);
                }
                {
                  local o = Fl_Check_Button(205, 390, 82, 20, _tr("Activo"));
                  articulos_activo = o;
                  o.labelsize(16);
                  o.down_box(FL_DOWN_BOX);
                  add_input_field_to_map("articulos", "activo", articulos_activo);
                }
                {
                  local o = Fl_Button(660, 386, 120, 28, _tr("Guardar"));
                  btnGuardarArticulo = o;
                  o.when(8);
                  o.labelsize(16);
                }
                {
                  local o = Fl_Multiline_Input(90, 422, 690, 86, _tr("Obs."));
                  articulos_observaciones = o;
                  o.textsize(16);
                  o.align(7);
                  o.labelsize(16);
                  add_input_field_to_map("articulos", "observaciones", articulos_observaciones);
                }
                {
                  local o = Fl_Check_Button(297, 390, 95, 20, _tr("Unidades"));
                  articulos_activo = o;
                  o.labelsize(16);
                  o.down_box(FL_DOWN_BOX);
                  add_input_field_to_map("articulos", "activo", articulos_activo);
                }
                {
                  local o = Fl_Check_Button(402, 390, 95, 20, _tr("Peso"));
                  articulos_use_peso = o;
                  o.labelsize(16);
                  o.down_box(FL_DOWN_BOX);
                  add_input_field_to_map("articulos", "use_peso", articulos_use_peso);
                }
                {
                  local o = Fl_Check_Button(507, 390, 95, 20, _tr("Bultos"));
                  articulos_use_bultos = o;
                  o.labelsize(16);
                  o.down_box(FL_DOWN_BOX);
                  add_input_field_to_map("articulos", "use_bultos", articulos_use_bultos);
                }
              }
              o.end();
            }
            {
              local o = Fl_Button(540, 302, 120, 28, _tr("Actualizar"));
              btnActualizarArticulos = o;
              o.labelsize(16);
            }
            {
              local o = Fl_Button(670, 302, 120, 28, _tr("Importar"));
              btnImportarArticulos = o;
              o.labelsize(16);
            }
          }
          o.end();
        }
        {
          local o = Fl_Group(0, 25, 800, 504, _tr("Totales (F6)"));
          tab_totales = o;
          o.labelsize(16);
          o.labeltype(FL_NO_LABEL);
          {
            {
              local o = Fl_Data_Table(10, 35, 780, 202, _tr("@2 Totales @2"));
              grid_totales = o;
              o.textsize(16);
              o.align(17);
              o.box(FL_ENGRAVED_BOX);
              o.color(175);
              o.labelsize(16);
              o.end();
              Fl_Group.current().resizable(o);
            }
            {
              local o = Fl_Data_Table(10, 241, 780, 202, _tr("@2 Detalle del Total @2"));
              grid_detalle_totales = o;
              o.textsize(16);
              o.align(17);
              o.box(FL_ENGRAVED_BOX);
              o.color(175);
              o.labelsize(16);
              o.end();
            }
            {
              local o = Fl_Button(10, 453, 120, 28, _tr("Actualizar"));
              btnActualizarTotales = o;
              o.labelsize(16);
            }
            {
              local o = Fl_Button(140, 453, 120, 28, _tr("Imprimir"));
              btnImprimirTotales = o;
              o.labelsize(16);
            }
            {
              local o = Fl_Button(670, 453, 120, 28, _tr("Crear Cierre"));
              btnCrearCierreTotales = o;
              o.labelsize(16);
            }
          }
          o.end();
        }
        {
          local o = Fl_Group(0, 25, 800, 504, _tr("Opciones (F9)"));
          tab_opciones = o;
          o.labelsize(16);
          o.labeltype(FL_NO_LABEL);
          {
            {
              local o = Fl_Group(10, 60, 255, 95, _tr("Impresión"));
              grupo_opciones_impresora = o;
              o.labelsize(18);
              o.color(246);
              o.box(FL_ENGRAVED_BOX);
              {
                {
                  local o = Fl_Check_Button(20, 70, 235, 28, _tr("Preguntar por impresora"));
                  preguntar_por_impresora = o;
                  o.labelsize(16);
                  o.down_box(FL_DOWN_BOX);
                }
                {
                  local o = Fl_Round_Button(20, 100, 100, 20, _tr("Papel A4"));
                  imprimir_papel_A4 = o;
                  o.labelsize(16);
                  o.value(1);
                  o.type(102);
                  o.down_box(FL_ROUND_DOWN_BOX);
                }
                {
                  local o = Fl_Round_Button(20, 125, 100, 20, _tr("Papel A5"));
                  imprimir_papel_A5 = o;
                  o.labelsize(16);
                  o.type(102);
                  o.down_box(FL_ROUND_DOWN_BOX);
                }
              }
              o.end();
            }
            {
              local o = Fl_Button(10, 165, 65, 28, _tr("Fecha"));
              btnFechaTrabajo = o;
              o.labelsize(16);
              o.hide();
            }
            {
              local o = Fl_Input(85, 165, 118, 28);
              fecha_de_trabajo = o;
              o.textsize(16);
              o.labeltype(FL_NO_LABEL);
              o.labelsize(16);
              o.hide();
            }
            {
              local o = Fl_Multiline_Input(10, 224, 251, 258, _tr("Envases (uno por linea)"));
              lista_de_envases = o;
              o.textsize(16);
              o.align(5);
              o.labelsize(16);
            }
            {
              local o = Fl_Button(10, 492, 251, 28, _tr("Guardar Envases"));
              btnGuardarEnvases = o;
              o.labelsize(16);
            }
          }
          o.end();
        }
      }
      o.end();
      Fl_Group.current().resizable(o);
    }
    {
      local o = Fl_Group(0, 530, 800, 30);
      status_bar = o;
      o.box(FL_ENGRAVED_BOX);
      o.labelsize(16);
      o.align(20);
      o.color(7);
      o.end();
    }
  }
}

class AyudaWindow extends Fl_Double_Window {
  
  // Declaration of class members
  ayuda_texto = null;
  btnGuardarAyuda = null;
  
  constructor(px=31, py=51, pw=470, ph=390, pl=_tr("Ayuda")){
    base.constructor(px, py, pw, ph, pl);
    // Create member functions and widgets
    {
      local o = Fl_Multiline_Input(10, 25, 452, 313, _tr("Ayuda"));
      ayuda_texto = o;
      o.textsize(16);
      o.align(5);
      o.labelsize(16);
    }
    {
      local o = Fl_Button(178, 348, 115, 33, _tr("Guardar"));
      btnGuardarAyuda = o;
      o.labelsize(16);
    }
  }
}

class Ayuda2Window extends Fl_Double_Window {
  
  // Declaration of class members
  texto_a_buscar = null;
  btnBuscarAyuda = null;
  btnGuardarAyuda = null;
  ayuda_texto = null;
  
  constructor(px=75, py=157, pw=565, ph=390, pl=_tr("Ayuda")){
    base.constructor(px, py, pw, ph, pl);
    // Create member functions and widgets
    {
      local o = Fl_Group(10, 348, 549, 34);
      {
        {
          local o = Fl_Input(10, 349, 299, 33, _tr("Texto a buscar"));
          texto_a_buscar = o;
          o.textsize(18);
          o.align(5);
          o.labelsize(18);
          Fl_Group.current().resizable(o);
        }
        {
          local o = Fl_Button(319, 349, 115, 33, _tr("Buscar"));
          btnBuscarAyuda = o;
          o.labelsize(16);
        }
        {
          local o = Fl_Button(444, 349, 115, 33, _tr("Guardar"));
          btnGuardarAyuda = o;
          o.labelsize(16);
        }
      }
      o.end();
    }
    {
      local o = Fl_Text_Editor_Buffered(10, 10, 550, 308);
      ayuda_texto = o;
      o.labelsize(18);
      o.textsize(18);
      Fl_Group.current().resizable(o);
    }
  }
}
