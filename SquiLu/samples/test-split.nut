local str = [=[
OurDeliveryCalc *delivery_calc_window;
EntitiesListSearchWindow *entities_list_window;
ProductsListSearchWindow *products_list_window;
CalendarWindow *calendar_window;]=];

local ar = str.split(';');
print(ar.len());
foreach(a in ar) print("xxx" + a + "zzz");