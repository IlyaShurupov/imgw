
#include "ImGuiObjectEditor.h"

#ifdef IMGW_OBJECTS_LIB

#include "imgui_internal.h"

using namespace ImGui;

void imgui_object_preview(obj::Object* childo, obj::Object*& clipboard, obj::objects_api* oh);

#ifdef IMGW_OBJECTS_LIB
#include "nodes.h"

void imgui_nodes_operator_info(nd::Operator* op) {
	if (ImGui::TreeNode("Description")) {
		ImGui::Text(op->getDescription().cstr());
		ImGui::TreePop();
	}

	if (ImGui::TreeNode("CallBacks")) {
		for (auto op_callback : *op->getCallbacks()) {
			if (ImGui::TreeNode(op_callback->key.cstr())) {

				ImGui::Text("Description : "); ImGui::SameLine();
				if (op_callback->val->getDescription()) {
					ImGui::Text(op_callback->val->getDescription());
				} else {
					ImGui::Text("Not given");
				}

				ImGui::Text("Arguments : "); ImGui::SameLine();
				for (auto arg : *op_callback->val->getInterface()) {
					ImGui::Text(arg->key.cstr()); ImGui::SameLine(); ImGui::Text("; ");
					ImGui::SameLine();
				}
				ImGui::Text(" ");

				ImGui::TreePop();
			}
		}
		ImGui::TreePop();
	}
}

bool imgui_nodes_operator_selector(nd::Operator*& out_op, const obj::ObjectType*& out_type, obj::TypeGroups* type_group = NULL) {

	if (!type_group) {
		type_group = &obj::NDO->type_groups;
	}

	for (auto childo : *type_group->getChilds()) {

		if (childo->val->isGroup()) {
			if (ImGui::BeginMenu((tp::string("Group : ") + childo->key).cstr())) {

				if (imgui_nodes_operator_selector(out_op, out_type, childo->val)) {
					ImGui::EndMenu();
					return true;
				}

				ImGui::EndMenu();
			}
			continue;
		}

		if (ImGui::BeginMenu((tp::string("Type : ") + childo->key).cstr())) {

			nd::TypeOperators* operators = (nd::TypeOperators*) childo->val->getType()->nodes_custom_data;
			if (operators) {

				for (auto op : *operators->getDict()) {
					
					ImGui::PushID(op.entry_idx);
					if (ImGui::Button("Select")) {
						out_type = childo->val->getType();
						out_op = op->val;
						ImGui::TreePop();
						ImGui::EndMenu();
						return true;
					}
					ImGui::PopID();

					ImGui::SameLine();

					if (ImGui::TreeNode(op->key.cstr())) {
						ImGui::Text(op->val->getDescription().cstr());
						ImGui::TreePop();
					}


				}

			} else {
				ImGui::Text("No Operators");
			}

			ImGui::EndMenu();
		}
	}

	return NULL;
}

object_path nodes_requesto_view(nd::Requester* in, obj::Object*& clipboard, obj::objects_api* oh) {

	if (in->op_type) {

		ImGui::Text("%s", in->op->getDescription().cstr());

		for (auto callback : in->callbacks_arguments->items) {

			if (ImGui::Button("Exeucte", {60, 27})) {
				in->call(callback->key);
			}

			ImGui::SameLine();

			if (ImGui::TreeNode(callback->key.cstr())) {

				NDO_CASTV(obj::DictObject, callback->val, args);
				assert(args);
				for (auto arg : args->items) {
					ImGui::Text(arg->key.cstr()); ImGui::SameLine();
					ImGui::PushID(arg->key.cstr());
					imgui_object_preview(arg->val, clipboard, oh);
					ImGui::PopID();
				}

				ImGui::TreePop();
			}
		}

		ImGui::Separator();
		if (ImGui::Selectable("Deinitialize requester")) {
			in->setNoOperator();
		}

		ImGui::Separator();
		if (ImGui::TreeNode(" Additional Operator Info ")) {
			imgui_nodes_operator_info(in->op);
			ImGui::TreePop();
		}

	} else {

		if (ImGui::BeginMenu("Select Target Operator")) {
			nd::Operator* out_op = NULL;
			const obj::ObjectType* out_type = NULL;
			if (imgui_nodes_operator_selector(out_op, out_type)) {
				in->setTargetOperator(out_type, out_op);
			}
			ImGui::EndMenu();
		}
	}

	return object_path();
}
#endif

object_path enumo_view(obj::EnumObject* in, obj::Object*& clipboard, obj::objects_api* oh) {

	if (!in->entries) {
		ImGui::Text("enum is uninitialized");
		return object_path();
	}

	if (ImGui::BeginCombo(" ", in->getActiveName())) {

		for (tp::uhalni idx = 0; idx < in->nentries; idx++) {
			if (ImGui::Selectable(in->getItemName(idx))) {
				obj::NDO->set(in, tp::alni(idx));
			}
		}

		ImGui::EndCombo();
	}

	return object_path();
}

object_path boolo_view(obj::Object* in, obj::Object*& clipboard, obj::objects_api* oh) {
	tp::alni& val = NDO_CAST(obj::BoolObject, in)->val;
	bool gui_val = bool(val);
	ImGui::Checkbox(gui_val ? "True" : "False", &gui_val);
	val = tp::alni(gui_val);
	return object_path();
}

object_path floato_view(obj::Object* in, obj::Object*& clipboard, obj::objects_api* oh) {
	tp::alnf& val = NDO_CAST(obj::FloatObject, in)->val;
	float gui_val = float(val);
	ImGui::InputFloat("Value", &gui_val);
	val = tp::alnf(gui_val);
	return object_path();
}

object_path nullo_view(obj::Object* in, obj::Object*& clipboard, obj::objects_api* oh) {
	ImGui::Text("Null Object.");
	return object_path();
}

object_path linko_view(obj::LinkObject* in, obj::Object*& clipboard, obj::objects_api* oh) {

	if (in->link) {

		ImGui::Text("%s at %x", in->link->type->name.cstr(), in->link);

		if (ImGui::Selectable("View")) {
			return {in->link, "adress"};
		}
	} else {
		ImGui::Text("Link Is Null");
	}

	if (ImGui::Selectable("Set from clipboard")) {
		in->link = clipboard;
	}

	if (ImGui::Selectable("Copy value")) {
		clipboard = in->link;
	}
	if (ImGui::Selectable("Set Null")) {
		in->link = NULL;
	}

	return object_path();
}

object_path into_view(obj::IntObject* in, obj::Object*& clipboard, obj::objects_api* oh) {
	ImGui::Text("Int Value: "); ImGui::SameLine();
	int val = (int) in->val;
	ImGui::InputInt(" ", &val);
	in->val = (tp::alni) val;
	return object_path();
}

void imgui_object_preview(obj::Object* childo, obj::Object*& clipboard, obj::objects_api* oh) {

	if (NDO_CAST(obj::LinkObject, childo)) {
		NDO_CASTV(obj::LinkObject, childo, linko);
		if (linko->link) {
			ImGui::Text("=> ");
			ImGui::SameLine();
			imgui_object_preview(linko->link, clipboard, oh);
		} else {
			ImGui::Text("Link Is Null");
		}

	} else if (NDO_CAST(obj::IntObject, childo)) {
		tp::alni& val = NDO_CAST(obj::IntObject, childo)->val;
		int gui_val = int(val);
		ImGui::InputInt(" ", &gui_val);
		val = tp::alni(gui_val);

	} else if (NDO_CAST(obj::StringObject, childo)) {
		NDO_CASTV(obj::StringObject, childo, stringo);
		static char val[2048] = {" "};
		if (stringo->val != val) { tp::memcp(val, stringo->val.cstr(), stringo->val.size() + 1); }
		ImGui::InputText("", val, 2048);
		if (stringo->val != val) { stringo->val = tp::string(val).capture(); }

	} else if (NDO_CAST(obj::BoolObject, childo)) {
		boolo_view((obj::BoolObject*) childo, clipboard, obj::NDO);

	} else if (NDO_CAST(obj::FloatObject, childo)) {
		tp::alnf& val = NDO_CAST(obj::FloatObject, childo)->val;
		float gui_val = float(val);
		ImGui::InputFloat(" ", &gui_val);
		val = tp::alnf(gui_val);

	} else if (NDO_CAST(obj::EnumObject, childo)) {
		enumo_view((obj::EnumObject*) childo, clipboard, obj::NDO);

	} else {
		ImGui::Text(childo->type->name.cstr()); ImGui::SameLine();
		ImGui::Text(" (No Preview) ");
	}
}

obj::Object* imgui_object_create_menu(obj::TypeGroups* type_group = NULL) {

	if (!type_group) {
		type_group = &obj::NDO->type_groups;
	}

	for (auto childo : *type_group->getChilds()) {

		if (childo->val->isGroup()) {
			if (ImGui::BeginMenu((childo->key).cstr())) {
				obj::Object* newo = imgui_object_create_menu(childo->val);
				ImGui::EndMenu();

				if (newo) {
					return newo;
				}
			}
			continue;
		}

		if (ImGui::Button((childo->key).cstr(), {100, 0})) {
			return obj::NDO->create(childo->key);
		}
	}

	return NULL;
}

object_path dicto_view(obj::DictObject* active, obj::Object*& clipboard, obj::objects_api* oh) {

	bool popup = false;
	object_path out;

	ImGui::BeginChild("frame", {0, 0}, 0, ImGuiWindowFlags_NoBackground);

	if (!active->items.size()) {
		ImGui::Text("Dictinary Is Empty. ");
	}

	if (ImGui::BeginTable("Items", 2)) {

		ImGui::TableSetupColumn("name", ImGuiTableColumnFlags_WidthFixed, 120.0f);
		ImGui::TableSetupColumn("info");

		for (auto childo : active->items) {
			ImGui::TableNextRow(0, 36);

			ImGui::TableSetColumnIndex(0);
			if (ImGui::Selectable(childo.iter->key.cstr())) {
				out = {childo.iter->val,  childo.iter->val->type->name + " '" + childo.iter->key + "'"};
				break;
			}

			{
				if (ImGui::BeginPopupContextItem(childo.iter->key.cstr(), ImGuiPopupFlags_MouseButtonRight)) {
					popup = true;

					ImGui::Text("%s at %x", childo->val->type->name.cstr(), childo->val);

					static obj::Object* name_parent = NULL;
					static char name[100] = {"asdas"};

					if (name_parent != childo) {
						tp::memcp(name, childo->key.cstr(), childo->key.size() + 1);
						name_parent = childo;
					}

					if (ImGui::InputTextEx(" ", "new name", name, 100, {140 , 30}, ImGuiInputTextFlags_EnterReturnsTrue)) {
						tp::alni idx = active->items.presents(name);
						if (idx != -1) {
							Notify("Object with such name Already Exists");
						} else {
							active->items.remove(childo->key);
							active->items.put(tp::string(name).capture(), name_parent);
							name_parent = NULL;
							ImGui::EndPopup();
							break;
						}
					}

					if (ImGui::Selectable("Exclude")) {
						active->items.remove(childo->key);
						ImGui::EndPopup();
						break;
					}

					if (ImGui::Selectable("Destroy")) {
						oh->destroy(childo->val);
						active->items.remove(childo->key);
						ImGui::EndPopup();
						break;
					}

					if (ImGui::Selectable("Copy Link")) {
						clipboard = childo.iter->val;
						ImGui::EndPopup();
						break;
					}

					name_parent = childo;
					ImGui::EndPopup();
				}
			}

			ImGui::TableSetColumnIndex(1);

			ImGui::PushID(childo.entry_idx);
			imgui_object_preview(childo->val, clipboard, oh);
			ImGui::PopID();
		}
		ImGui::EndTable();
	}

	ImGui::EndChild();

	if (!popup && ImGui::BeginPopupContextItem("child_2", ImGuiPopupFlags_MouseButtonRight)) {

		if (ImGui::Selectable("Paste")) {

			tp::alni idx = 1;
			tp::string name_base = tp::string("clipboard ") + clipboard->type->name + tp::string(" ");
			tp::string name_out = name_base;

			while (active->items.presents(name_out)) {
				name_out = name_base + tp::string(idx);
				idx++;
			}

			active->items.put(name_out, clipboard);
		}

		if (ImGui::BeginMenu("Create")) {

			obj::Object* newo = imgui_object_create_menu();

			if (newo) {
				tp::alni idx = 1;
				tp::string name_base = tp::string("new ") + newo->type->name + tp::string(" ");
				tp::string name_out = name_base;

				while (active->items.presents(name_out)) {
					name_out = name_base + tp::string(idx);
					idx++;
				}

				active->items.put(name_out, newo);
			}

			ImGui::EndMenu();
		}

		ImGui::EndPopup();
	}

	return out;
}

object_path listo_view(obj::ListObject* active, obj::Object*& clipboard, obj::objects_api* oh) {
	bool popup = false;
	object_path out;

	if (!active->items.length()) {
		ImGui::Text("List Is Empty. ");
	}

	ImGui::BeginChild("frame", {0, 0}, 0, ImGuiWindowFlags_NoBackground);

	if (ImGui::BeginTable("Items", 2)) {
		ImGui::TableSetupColumn("type", ImGuiTableColumnFlags_WidthFixed, 120.0f);
		ImGui::TableSetupColumn("info");

		for (auto childo : active->items) {
			ImGui::TableNextRow(0, 36);

			ImGui::TableSetColumnIndex(0);
			if (ImGui::Selectable(tp::string(childo.idx()).cstr())) {
				out = object_path(childo.data(), childo->type->name + " at " + tp::string(childo.idx()));
				break;
			}

			{
				if (ImGui::BeginPopupContextItem(tp::string(childo.idx()).cstr(), ImGuiPopupFlags_MouseButtonRight)) {
					popup = true;

					ImGui::Text("%s at %x", childo->type->name.cstr(), childo.data());

					if (ImGui::Selectable("Exclude")) {
						active->items.delNode(childo.node());
						ImGui::EndPopup();
						break;
					}

					if (ImGui::Selectable("Copy Link")) {
						clipboard = childo.data();
						ImGui::EndPopup();
						break;
					}

					if (childo.node()->prev && ImGui::Selectable("Move Up")) {
						SWAP(childo.node()->prev->data, childo.data(), obj::Object*);
						ImGui::EndPopup();
						break;
					}

					if (childo.node()->next && ImGui::Selectable("Move Down")) {
						SWAP(childo.node()->next->data, childo.data(), obj::Object*);
						ImGui::EndPopup();
						break;
					}

					if (ImGui::Selectable("Destroy")) {
						oh->destroy(childo.data());
						active->items.delNode(childo.node());
						ImGui::EndPopup();
						break;
					}

					ImGui::EndPopup();
				}
			}

			ImGui::TableSetColumnIndex(1);

			ImGui::PushID(childo.idx());
			imgui_object_preview(childo.data(), clipboard, oh);
			ImGui::PopID();

		}
		ImGui::EndTable();
	}

	ImGui::EndChild();

	if (!popup && ImGui::BeginPopupContextItem("child_2", ImGuiPopupFlags_MouseButtonRight)) {

		if (ImGui::Selectable("Paste")) {
			active->items.pushBack(clipboard);
		}

		if (ImGui::BeginMenu("Create")) {

			obj::Object* newo = imgui_object_create_menu();
			if (newo) active->items.pushBack(newo);

			ImGui::EndMenu();
		}
		ImGui::EndPopup();
	}

	return out;
}

object_path stringo_view(obj::StringObject* in, obj::Object*& clipboard, obj::objects_api* oh) {

	ImGui::Text("String Data: ");

	static char val[2048] = {" "};

	if (in->val != val) {
		tp::memcp(val, in->val.cstr(), in->val.size() + 1);
	}

	ImGui::InputTextMultiline(" ", val, 2048, {ImGui::GetWindowContentRegionWidth() - 25, ImGui::GetWindowContentRegionWidth() * 1.1f});

	if (in->val != val) {
		in->val = tp::string(val).capture();
	}

	return object_path();
}

object_path classo_view(obj::ClassObject* in, obj::Object*& clipboard, obj::objects_api* oh) {
	if (ImGui::TreeNode("Members ")) {
		object_path path = dicto_view(in->members, clipboard, oh);

		ImGui::TreePop();
		return path;
	}

	return object_path();
}

void ImGuiObjectEditor::oexplorer(tp::halnf width) {

	//ImGui::Text("View Path: "); ImGui::SameLine();
	ImGui::BeginChild("child_path", {0, 45}, false, ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoBackground | ImGuiWindowFlags_HorizontalScrollbar);
	tp::Stack<object_path*> rev_path;
	for (auto childo = view_path.last; childo; childo = childo->prev) {
		rev_path.push(&childo->data);
	}
	tp::alni idx = 0;
	for (auto childo = rev_path.last; childo; childo = childo->prev) {
		ImGui::PushID((int) idx);
		bool go_back = false;
		if (childo == rev_path.last) {
			go_back = ImGui::Button(childo->data->id.cstr()); ImGui::SameLine();
		} else {
			go_back = ImGui::Button((childo->data->id).cstr()); ImGui::SameLine();
		}
		ImGui::PopID();

		if (ImGui::BeginPopupContextItem(NULL, ImGuiPopupFlags_MouseButtonRight)) {

			obj::Object* curretn_object = childo->data->obj;
			ImGui::Text("%s at %x", curretn_object->type->name.cstr(), curretn_object);
			ImGui::Separator();

			if (ImGui::Selectable("Copy Link")) {
				clipboard = curretn_object;
			}

			if (ImGui::Selectable("Instantiate  ")) {
				clipboard = obj::NDO->create(curretn_object->type->name.cstr());
				obj::NDO->copy(clipboard, curretn_object);
				Notify("Object copied to clipboard");
			}

			ImGui::Separator();

			static char path_str[100] = {"data.o\0"};
			ImGui::InputTextEx(" ", "save path", path_str, 100, {100, 30}, 0);

			bool save_object = ImGui::Selectable("Save Object");
			bool load_object = ImGui::Selectable("Load Object");

			if (save_object) {
				obj::NDO->save(curretn_object, path_str);
				Notify("Object saved");
			}

			if (load_object) {
				obj::Object* loadedo = obj::NDO->load(path_str);
				if (loadedo) {
					clipboard = loadedo;
					Notify("Object copied to clipboard");
				} else {
					Notify("Can't load Object");
				}
			}

			ImGui::EndPopup();
		}

		if (go_back) {
			while (&view_path.last->data != childo->data) {
				view_path.pop();
			}
			active = childo->data->obj;
			ImGui::EndChild();
			return;
		}
		idx++;
	}
	ImGui::Text(" ");
	ImGui::EndChild();

	ImGui::Separator();

	object_path new_active;
	if (NDO_CAST(obj::NullObject, active)) {
		new_active = nullo_view(active, clipboard, obj::NDO);
	} else if (NDO_CAST(obj::LinkObject, active)) {
		new_active = linko_view((obj::LinkObject*) active, clipboard, obj::NDO);
	} else if (NDO_CAST(obj::IntObject, active)) {
		new_active = into_view((obj::IntObject*) active, clipboard, obj::NDO);
	} else if (NDO_CAST(obj::StringObject, active)) {
		new_active = stringo_view((obj::StringObject*) active, clipboard, obj::NDO);
	} else if (NDO_CAST(obj::ListObject, active)) {
		new_active = listo_view((obj::ListObject*) active, clipboard, obj::NDO);
	} else if (NDO_CAST(obj::DictObject, active)) {
		new_active = dicto_view((obj::DictObject*) active, clipboard, obj::NDO);
	} else if (NDO_CAST(obj::ClassObject, active)) {
		new_active = classo_view((obj::ClassObject*) active, clipboard, obj::NDO);
	} else if (NDO_CAST(obj::BoolObject, active)) {
		new_active = boolo_view((obj::BoolObject*) active, clipboard, obj::NDO);
	} else if (NDO_CAST(obj::FloatObject, active)) {
		new_active = floato_view((obj::FloatObject*) active, clipboard, obj::NDO);
	} else if (NDO_CAST(obj::EnumObject, active)) {
		new_active = enumo_view((obj::EnumObject*) active, clipboard, obj::NDO);
		#ifdef IMGW_OBJECTS_NODES_LIB
	} else if (NDO_CAST(nd::Requester, active)) {
		nodes_requesto_view((nd::Requester*) active, clipboard, obj::NDO);
		#endif
	} else {
		ImGui::Text("Preview is Unavaliable");
	}

	if (new_active != NULL) {
		active = new_active.obj;
		view_path.push(new_active);
	}
}

void ImGuiObjectEditor::oproperties(const obj::ObjectType* type, bool top_of_tree_vew) {

	assert(type);

	ImGui::Text("  Type : %s", type->name.cstr());

	if (ImGui::TreeNode("Description ")) {
		if (type->description != " ") {
			ImGui::Text(type->description.cstr());
		} else {
			ImGui::Text("Description is not given");
		}
		ImGui::TreePop();
	}

	if (ImGui::TreeNode("Size")) {
		ImGui::Text("Struct Size: %i bytes", type->size);
		if (type->save_size) {
			ImGui::Text("Save Size: %i bytes", type->save_size(active));
		} else {
			ImGui::Text("Save Size: none (not defined)");
		}
		ImGui::TreePop();
	}

	if (ImGui::TreeNode("Methods")) {

		if (type->methods) {
			for (tp::alni idx = 0; type->methods[idx].adress; idx++) {
				obj::type_method* method = &type->methods[idx];
				ImGui::PushID(method->name.cstr());
				ImGui::Text("%s", method->name.cstr());
				ImGui::PopID();
			}
		} else {
			ImGui::Text("type has no methods");
		}

		ImGui::TreePop();
	}

	#ifdef IMGW_OBJECTS_NODES_LIB
	if (ImGui::TreeNode("Operators")) {
		if (type->nodes_custom_data) {
			nd::TypeOperators* operators = (nd::TypeOperators*) type->nodes_custom_data;
			for (auto op : *operators->getDict()) {
				if (ImGui::TreeNode(op->key.cstr())) {

					imgui_nodes_operator_info(op->val);

					ImGui::TreePop();
				}
			}
		} else {
			ImGui::Text("No operators");
		}

		ImGui::TreePop();
	}
	#endif

	if (ImGui::TreeNode("Base Type")) {
		if (type->base) {
			oproperties(type->base, false);
		} else {
			ImGui::Text("No base type");
		}
		ImGui::TreePop();
	}
}

void ImGuiObjectEditor::Draw() {

	if (WindowEditor("Explorer")) {
		oexplorer(ImGui::GetWindowContentRegionWidth());
	}
	ImGui::End();

	if (WindowEditor("Type Info")) {
		oproperties(active->type);
	}
	ImGui::End();
}

#endif