#include "dr_api.h"
#include "drmgr.h"
#include "drreg.h"

static int fault_label_note;

static void
insert_faulting_taken_log(void *drcontext, instrlist_t *bb, instr_t *label)
{
    reg_id_t reg_ptr;
    instr_t *where = instr_get_next(label);

    if (drreg_reserve_register(drcontext, bb, where, NULL, &reg_ptr) != DRREG_SUCCESS)
        DR_ASSERT(false);

    instrlist_insert_mov_immed_ptrsz(drcontext, 1, opnd_create_reg(reg_ptr), bb, where,
                                     NULL, NULL);
    instrlist_meta_preinsert(
        bb, where,
        INSTR_XL8(XINST_CREATE_store(drcontext, OPND_CREATE_MEM64(reg_ptr, 0),
                                 opnd_create_reg(reg_ptr)), instr_get_app_pc(where)));

    drreg_unreserve_register(drcontext, bb, where, reg_ptr);
}

static dr_emit_flags_t
event_bb_app2app(void *drcontext, void *tag, instrlist_t *bb, bool for_trace,
                 bool translating)
{
    instr_t *branch = instrlist_last_app(bb);
    if (branch == NULL || instr_get_opcode(branch) != OP_b)
        return DR_EMIT_DEFAULT;

    app_pc current_pc = instr_get_app_pc(branch);
    instr_t* fault_label = INSTR_XL8(INSTR_CREATE_label(drcontext), current_pc);
    instr_set_note(fault_label, &fault_label_note);
    instrlist_preinsert(bb, branch, fault_label);

    return DR_EMIT_STORE_TRANSLATIONS;
}

static dr_emit_flags_t
event_bb_insertion(void *drcontext, void *tag, instrlist_t *bb, instr_t *instr,
                   bool for_trace, bool translating, void *user_data)
{
    if (instr_is_label(instr) && instr_get_note(instr) == &fault_label_note)
        insert_faulting_taken_log(drcontext, bb, instr);
    return DR_EMIT_DEFAULT;
}

static void
event_exit(void)
{
    drreg_exit();
    drmgr_exit();
}

DR_EXPORT
void
dr_client_main(client_id_t id, int argc, const char *argv[])
{
    drreg_options_t ops = { sizeof(ops), 4, false };
    dr_set_client_name("regspill MRE", "https://example.com/");
    drmgr_init();
    drreg_init(&ops);
    drmgr_register_exit_event(event_exit);
    if (!drmgr_register_bb_app2app_event(event_bb_app2app, NULL) ||
        !drmgr_register_bb_instrumentation_event(NULL, event_bb_insertion, NULL))
        DR_ASSERT(false);
}
