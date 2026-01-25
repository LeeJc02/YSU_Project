new Vue({
    el: '#app',
    data: {
        recipient: '',
        amount: '',
        message: ''
    },
    methods: {
        goBack() {
            window.history.back();
        },
        transfer() {
            if (this.amount <= 0) {
                this.message = '转账金额必须大于0';
                return;
            }
            if (!this.recipient) {
                this.message = '请填写收款人账户';
                return;
            }
            axios.post('/account/transfer', { target_card_number : this.recipient, amount: this.amount })
                .then(response => {
                    this.message = '转账成功！';
                    // 可以在此更新余额或跳转页面
                })
                .catch(error => {
                    this.message = '转账失败，请稍后再试';
                });
        }
    }
});